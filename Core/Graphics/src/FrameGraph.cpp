#include <FrameGraph.h>

#include <queue>
#include <unordered_map>

#include <DescriptorPoolBuilder.h>
#include <ImageBuilder.h>
#include <ImageResourceNode.h>
#include <PipelineBuilder.h>
#include <RenderPassBuilder.h>
#include <RenderPassNode.h>
#include <DepthStencilImageResourceNode.h>
#include <UniformResourceNode.h>

FrameGraph::FrameGraph(IntrusivePtr<VulkanContext>& context, IntrusivePtr<SwapChain> swapChain)
        : context(context), swapChain(swapChain) {}

FrameGraph::~FrameGraph() { }

void FrameGraph::Link(IntrusivePtr<FrameGraphNode>&& from, IntrusivePtr<FrameGraphNode>&& to) {
    nodesInMap[to].push_back(from);
    nodesOutMap[from].push_back(to);
    from->refCount++;
}

void Add(IntrusivePtr<FrameGraphNode>&& node) {

}

void FrameGraph::TopoSort() {
    levelResult.resize(passNodes.size() + attachmentNodes.size());

    // nodes in degree counter map
    std::unordered_map<IntrusivePtr<FrameGraphNode>, uint32_t> counterMap;
    std::queue<IntrusivePtr<FrameGraphNode>> sortQueue;
    for (auto& [k, v] : nodesInMap) {
        counterMap[k] = v.size();
        if (v.empty()) {
            sortQueue.push(k);
        }
    }

    uint32_t levelCounter = sortQueue.size();
    uint32_t currentLevel = 0;
    while (!sortQueue.empty()) {
        auto front = sortQueue.front();
        sortQueue.pop();
        levelCounter--;

        // only valid node get pushed in result
        if (front->refCount != 0) {
            levelResult[currentLevel].push_back(front);
        }

        auto& outs = nodesOutMap[front];
        for (auto& out : outs) {
            counterMap[out]--;
            if (counterMap[out] == 0) {
                sortQueue.push(out);
            }
        }

        if (!levelCounter) {
            levelCounter = sortQueue.size();
            currentLevel++;
        }
    }

    levelResult.resize(currentLevel);

#ifndef NDEBUG
    for (int i = 0; i < levelResult.size(); i++) {
        spdlog::info("lv: {}", i);
        for (auto v : levelResult[i]) {
            spdlog::info("	name: {}", v->name.c_str());
        }
    }

    for (auto [k, v] : counterMap) {
        spdlog::info("{} {}", k->name.c_str(), v);
        if (v != 0) {
            throw std::runtime_error("Cyclic graph");
        }
    }
#endif
}

void FrameGraph::Cull() {
    for (auto& [k, v] : this->nodesOutMap) {
        if (v.empty() && k->refCount == 0) {
            TraceBack(k);
        }
    }

    printNode();
}

void FrameGraph::TraceBack(const IntrusivePtr<FrameGraphNode>& node) {
    if (node->refCount > 0) {
        node->refCount--;
    }
    auto froms = this->nodesInMap[node];
    for (auto& from : froms) {
        TraceBack(from);
    }
}

void FrameGraph::BuildRenderPass() {
    auto renderPassBuilder = RenderPassBuilder(context);

    std::vector<VkAttachmentDescription> vads;
    // imageNode -> index
    std::unordered_map<IntrusivePtr<FrameGraphNode>, uint32_t> vadsMap;
    std::unordered_map<IntrusivePtr<FrameGraphNode>,
                       std::vector<VkPipelineColorBlendAttachmentState>>
            passColorAttachmentBlendStates;

    std::vector<VkClearValue> attachmentClearValues;

    for (uint32_t i = 0; i < attachmentNodes.size(); i++) {
        auto imageResourceNode = static_cast<ImageResourceNode*>(attachmentNodes[i].get());
        vadsMap[imageResourceNode] = i;
        vads.push_back(imageResourceNode->vad);
        attachmentClearValues.push_back(imageResourceNode->clearValue);
    }

    renderPassBuilder.SetClearValues(attachmentClearValues);
    renderPassBuilder.SetAttachments(vads);

    for (uint32_t i = 0; i < passNodes.size(); i++) {
        std::vector<VkAttachmentReference> colorRefs;
        std::vector<VkPipelineColorBlendAttachmentState> colorBlendStates;
        std::vector<VkAttachmentReference> inputRefs;
        std::vector<VkAttachmentReference> depthRef;

        auto renderPassNode = static_cast<RenderPassNode*>(passNodes[i].get());

        bool haveSwapChainAttachment = false;
        // record color 
        for (auto& [node, binding] : renderPassNode->colorImageNodes) {
            auto imageResourceNode = node->As<ImageResourceNode>();
            if (imageResourceNode->isSwapChainImage) {
                haveSwapChainAttachment = true;
            }

            colorRefs.push_back(
                    {vadsMap[imageResourceNode], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});
            passColorAttachmentBlendStates[renderPassNode].push_back(imageResourceNode->blendState);
        }

        // record input
        for (auto& [node, binding] : renderPassNode->inputImageNodes) {
            auto imageResourceNode = node->As<ImageResourceNode>();
            inputRefs.push_back(
                    {vadsMap[imageResourceNode], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
        }

        // record depth stencil
        if (renderPassNode->depthStencilImageNode) {
            depthRef.push_back({vadsMap[renderPassNode->depthStencilImageNode],
                                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL});
        }

        renderPassBuilder.AddSubPass(VK_PIPELINE_BIND_POINT_GRAPHICS,
                                     std::move(colorRefs),
                                     std::move(depthRef),
                                     std::move(inputRefs));

        bool depRequired = false;
        VkSubpassDependency dep = {};
        if (haveSwapChainAttachment) {
            VkSubpassDependency dep = {};
            dep.srcSubpass = VK_SUBPASS_EXTERNAL;
            dep.dstSubpass = i;
            dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dep.srcAccessMask = 0;
            dep.dstAccessMask =
                    VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            dep.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            renderPassBuilder.AddDependency(dep);
        }

        // handle RAW, WAW, WAR
        if (renderPassNode->inputImageNodes.size() > 0) {
            dep.srcStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dep.dstStageMask |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            dep.srcAccessMask |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            dep.dstAccessMask |= VK_ACCESS_SHADER_READ_BIT;
            depRequired = true;
        }

        dep.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        if (depRequired) {
            dep.srcSubpass = i - 1;
            dep.dstSubpass = i;
            renderPassBuilder.AddDependency(dep);
        }
    }

    renderPassBuilder.SetSwapChainSize(swapChain->Size());
    this->renderPass = renderPassBuilder.Build();

    // build pipelines
    for (int i = 0; i < passNodes.size(); i++) {
        auto rpNode = static_cast<RenderPassNode*>(passNodes[i].get());
        if (!rpNode->pipelineSetupFn) {
            continue;
            throw std::runtime_error("pipelineSetupFn required");
        }

        auto builder =
                PipelineBuilder(context)
                        .SetRenderPass(renderPass, i)
                        .SetBlendAttachmentState(std::move(passColorAttachmentBlendStates[rpNode]))
                        .SetRenderPass(renderPass, i);
        auto pipeline = rpNode->pipelineSetupFn(builder);
        renderPass->GetPipelines().push_back(pipeline);
    }

    DescriptorPoolBuilder(context)
            .SetPipelines(renderPass->GetPipelines())
            .SetRenderPass(renderPass)
            .SetSwapChainSize(swapChain->Size())
            .Build();
}

void FrameGraph::Compile() {
    Cull();
    TopoSort();
    BuildRenderPass();
}

// resolve resource in node 
// update related descriptor set in pipeline
class ResourceDescriptorResolveVisitor : public ResourceNodeVisitor {
public:
    ResourceDescriptorResolveVisitor(Pipeline* pipeline, VkExtent3D swapChainExtent, uint32_t binding, uint32_t swapChainSize)
            : pipeline(pipeline)
            , swapChainExtent(swapChainExtent)
            , binding(binding)
            , swapChainSize(swapChainSize) {}

    virtual void Visit(ImageResourceNode* node) {
        node->Resolve(swapChainExtent, swapChainSize);

        auto imageViews = node->GetImageViews();
        std::vector<VkWriteDescriptorSet> writeDescriptorSets;
        std::vector<std::vector<VkDescriptorImageInfo>> descriptorImageInfos;
        descriptorImageInfos.resize(swapChainSize);

        for (int i = 0; i < imageViews.size(); i++)
        {
            auto imageView = imageViews[i];

            VkDescriptorImageInfo descriptorImageInfo = {};
            descriptorImageInfo.sampler = nullptr;
            descriptorImageInfo.imageView = imageView->GetView();
            descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            descriptorImageInfos[i].push_back(descriptorImageInfo);

                
            VkWriteDescriptorSet writeDescriptorSet = {};
            writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            // write set 0 by default
            writeDescriptorSet.dstSet = pipeline->GetDescriptorSet(i, 0);
            writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
            writeDescriptorSet.dstBinding = binding;
            writeDescriptorSet.pImageInfo = descriptorImageInfos[i].data();
            // 1:1 binding
            writeDescriptorSet.descriptorCount = descriptorImageInfos[i].size();
            writeDescriptorSets.push_back(writeDescriptorSet);
        }

        vkUpdateDescriptorSets(pipeline->Context()->GetVkDevice(),
                               static_cast<uint32_t>(writeDescriptorSets.size()),
                               writeDescriptorSets.data(),
                               0,
                               nullptr);
    }

    virtual void Visit(DepthStencilImageResourceNode* node) {
        node->Resolve({swapChainExtent.height, swapChainExtent.width, 1}, swapChainSize);
    }

    virtual void Visit(UniformResourceNode* node) {
        // TD: allocate gpu buffer
        node->Resolve(swapChainExtent);

        std::vector<VkWriteDescriptorSet> writeDescriptorSets;
        std::vector<std::vector<VkDescriptorImageInfo>> descriptorImageInfos;
        descriptorImageInfos.resize(swapChainSize);

        for (int i = 0; i < swapChainSize; i++) {
            VkDescriptorBufferInfo descriptorBufferInfo = {};
            descriptorBufferInfo.buffer = node->GetGeneralBuffer()->GetBuffer();
            descriptorBufferInfo.offset = 0;
            descriptorBufferInfo.range = VK_WHOLE_SIZE;

            VkWriteDescriptorSet writeDescriptorSet = {};
            writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSet.dstSet = pipeline->GetDescriptorSet(i, 0);
            writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            writeDescriptorSet.dstBinding = binding;
            writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;
            writeDescriptorSet.descriptorCount = 1;

            writeDescriptorSets.push_back(writeDescriptorSet);
        }

        vkUpdateDescriptorSets(pipeline->Context()->GetVkDevice(),
                               static_cast<uint32_t>(writeDescriptorSets.size()),
                               writeDescriptorSets.data(),
                               0,
                               nullptr);
    }

private:
    VkExtent3D swapChainExtent;
    Pipeline* pipeline;
    uint32_t binding;
    uint32_t swapChainSize;
};

void FrameGraph::ResolveResource() {
    for (int i = 0; i < passNodes.size(); i++) {
        auto passNode = static_cast<RenderPassNode*>(passNodes[i].get());
        if (i >= renderPass->GetPipelines().size()) {
            continue;
        }
        auto& pipeline = renderPass->GetPipelines()[i];

        // allocate pipeline descriptor set
        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorPool = renderPass->GetDescriptorPool();
        descriptorSetAllocateInfo.pSetLayouts = pipeline->descriptorSetLayouts.data();
        descriptorSetAllocateInfo.descriptorSetCount = pipeline->descriptorSetLayouts.size();
        
        pipeline->descriptorSets.resize(swapChain->Size());
        // for each frame
        for (int j = 0; j < swapChain->Size(); j++)
        {
            // TODO: support multiple descriptor set
            pipeline->descriptorSets[j].resize(1);
            vkAllocateDescriptorSets(context->GetVkDevice(),
                                     &descriptorSetAllocateInfo,
                                     &pipeline->descriptorSets[j][0]);
        }

        auto swapChainExtent = swapChain->Extent();
        VkExtent3D extent = {swapChainExtent.height, swapChainExtent.width, 1};

        auto ResolveNodeMap = [&](std::unordered_map<IntrusivePtr<FrameGraphNode>, uint32_t>& nodes) {
            for (auto& [node, binding] : nodes) {
                ResourceDescriptorResolveVisitor visitor(pipeline.get(), extent, binding, swapChain->Size());
                static_cast<ResourceNode*>(node.get())->Accept(&visitor);
            }
        };

        // resolve uniform resource
        ResolveNodeMap(passNode->inputUniformNodes);

        // allocate descriptor for input attachments
        ResolveNodeMap(passNode->inputImageNodes);

        // color attachment do not require descriptor
        for (auto& [colorNode, binding] : passNode->colorImageNodes) {
            colorNode->As<ImageResourceNode>()->Resolve(extent, swapChain->Size());
        }

        if (passNode->resolveImageNode) {
            passNode->resolveImageNode->As<ImageResourceNode>()->Resolve(extent, swapChain->Size());
        }

        if (passNode->depthStencilImageNode) {
            passNode->depthStencilImageNode->As<ImageResourceNode>()->Resolve(extent, swapChain->Size());
        }

        for (auto& preserveNode : passNode->preserveImageNodes) {
            preserveNode->As<ImageResourceNode>()->Resolve(extent, swapChain->Size());
        }
    }

    for (int i = 0; i < swapChain->Size(); i++)
    {
        std::vector<VkImageView> attachmentViews;
        for (auto node : attachmentNodes) {
            auto imageResourceNode = static_cast<ImageResourceNode*>(node.get());
            attachmentViews.push_back(imageResourceNode->GetImageView(i)->GetView());
        }
        // setup framebuffer
        renderPass->BuildFrameuffer(attachmentViews, this->swapChain->Extent());
    }
}

void FrameGraph::Execute()
{
    auto currentFrameMod = currentFrame % swapChain->Size();
    
    uint32_t swapChainImageIndex = 0;
    auto result = vkAcquireNextImageKHR(context->GetVkDevice(),
                                        swapChain->GetSwapChain(),
                                        UINT64_MAX,
                                        renderPass->GetPresentSemaphores()[currentFrameMod],
                                        nullptr,
                                        &swapChainImageIndex);

    auto swapChainExtent = swapChain->Extent();
    auto& clearValues = renderPass->GetClearValue();

	VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = renderPass->GetRenderPass();
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent = swapChainExtent;
    renderPassBeginInfo.clearValueCount = clearValues.size();
    renderPassBeginInfo.pClearValues = clearValues.data();
    renderPassBeginInfo.framebuffer = renderPass->GetFrameBuffers()[swapChainImageIndex];

    auto commandBuffer = renderPass->GetGraphicCommandBuffers()[currentFrameMod];
    VkCommandBufferBeginInfo cmdBufferBeginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    vkBeginCommandBuffer(commandBuffer, &cmdBufferBeginInfo);

    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport = {};
    viewport.width = (float)swapChain->Extent().width;
    viewport.height = (float)swapChain->Extent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D rect2D = {};
    rect2D.extent.width = viewport.width;
    rect2D.extent.height = viewport.height;
    rect2D.offset.x = 0;
    rect2D.offset.y = 0;
    vkCmdSetScissor(commandBuffer, 0, 1, &rect2D);
    
    for (int i = 0; i < passNodes.size(); i++) {
        auto& passNode = passNodes[i];
        auto renderPassNode = static_cast<RenderPassNode*>(passNode.get());
        auto& pipeline = renderPass->GetPipelines()[i];

        vkCmdBindPipeline(commandBuffer,
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          renderPass->GetPipelines()[i]->GetPipeline());

        
        vkCmdBindDescriptorSets(commandBuffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                pipeline->GetPipelineLayout(),
                                0,
                                1,
                                &pipeline->GetDescriptorSet(i, 0),
                                0,
                                nullptr);

        renderPassNode->pipelineExecuteFn(commandBuffer);
    }

    vkCmdEndRenderPass(commandBuffer);

    vkEndCommandBuffer(commandBuffer);


    VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    submitInfo.pWaitDstStageMask = &submitPipelineStages;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &renderPass->GetPresentSemaphores()[swapChainImageIndex];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &renderPass->GetRenderSemaphores()[currentFrameMod];
    // Command buffer to be submitted to the queue
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;


    auto graphicQueue = context->GetQueue(VK_QUEUE_GRAPHICS_BIT).queue;
    // Submit to queue
    vkQueueSubmit(graphicQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkDeviceWaitIdle(context->GetVkDevice());

    VkPresentInfoKHR presentInfo = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    presentInfo.pNext = NULL;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapChain->GetSwapChain();
    presentInfo.pImageIndices = &swapChainImageIndex;
    presentInfo.pWaitSemaphores = &renderPass->GetRenderSemaphores()[currentFrameMod];
    presentInfo.waitSemaphoreCount = 1;

    result = vkQueuePresentKHR(graphicQueue, &presentInfo);

    vkDeviceWaitIdle(context->GetVkDevice());
    
    currentFrame++;
}

IntrusivePtr<VulkanContext>& FrameGraph::Context() { return context; }
