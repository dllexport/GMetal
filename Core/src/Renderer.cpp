#include <FrameGraph.h>
#include <ImageResourceNode.h>
#include <Material.h>
#include <PipelineBuilder.h>
#include <RenderPassNode.h>
#include <Renderer.h>
#include <ResourceNode.h>
#include <spdlog/spdlog.h>

Renderer::Renderer(IntrusivePtr<VulkanContext>& context) : context(context) {}

void Renderer::AddView(IntrusivePtr<IView>& view) {
    if (views.find(view) == views.end()) {
        views[view] = {};
    }
}

void Renderer::SetDefaultRenderPass(IntrusivePtr<RenderPass> renderPass) {
    this->defaultRenderPass = renderPass;
}

void Renderer::AddDrawable(IntrusivePtr<Drawable> drawable,
                           IntrusivePtr<IView>& view,
                           IntrusivePtr<RenderPass> renderPass) {
    if (!views.count(view)) {
        spdlog::warn("Renderer::AddDrawable view not found in views");
        return;
    }

    auto rp = renderPass;
    if (!rp) {
        rp = defaultRenderPass;
    }

    auto& dm = this->views[view];
    dm[rp].push_back(drawable);
}

void Renderer::StartFrame() {
    for (auto& [view, renderMap] : views) {
        auto swapChain = view->GetSwapChain();
        auto swapChainExtent = swapChain->Extent();
        auto swapChainImage = swapChain->GetImages()[frameNumber % 2];
        auto surfaceFormat = swapChain->GetSurfaceFormat();
        auto depthFormat = swapChain->DepthStencilFormat();

        auto fg = gmetal::make_intrusive<FrameGraph>(this->context);

        auto gbufferPass = fg->CreatePass<RenderPassNode>("gbuffer pass");
        gbufferPass->Setup([&](PipelineBuilder& builder) {
            return builder.SetVertexShader("shaders/gbuffer.vert.spv")
                    .SetFragmentShader("shaders/gbuffer.frag.spv")
                    .SetVertexInput({VertexComponent::Position,
                                     VertexComponent::Color,
                                     VertexComponent::Normal})
                    .SetRasterizer(VK_POLYGON_MODE_FILL,
                                   VK_CULL_MODE_BACK_BIT,
                                   VK_FRONT_FACE_COUNTER_CLOCKWISE)
                    .AddDescriptorSetLayoutBinding(
                            {PipelineBuilder::BuildDescriptorSetLayoutBinding(
                                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                    VK_SHADER_STAGE_VERTEX_BIT,
                                    0)})
                    .Build();
        });

        auto depth = fg->CreateImage<ImageResourceNode>("depth", depthFormat);
        depth->SetDepthStencil();

        auto color = fg->CreateImage<ImageResourceNode>("color", swapChainImage);
        color->SetSwapChainImage();

        auto normal = fg->CreateImage<ImageResourceNode>("normal", surfaceFormat.format);
        auto albedo = fg->CreateImage<ImageResourceNode>("albedo", surfaceFormat.format);
        auto position = fg->CreateImage<ImageResourceNode>("position", surfaceFormat.format);

        auto mergePass = fg->CreatePass<RenderPassNode>("merge pass");
        mergePass->Setup([&](PipelineBuilder& builder) {
            std::vector<VkDescriptorSetLayoutBinding> passBindings;
            passBindings.push_back(PipelineBuilder::BuildDescriptorSetLayoutBinding(
                    VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT, 0));
            passBindings.push_back(PipelineBuilder::BuildDescriptorSetLayoutBinding(
                    VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT, 1));
            passBindings.push_back(PipelineBuilder::BuildDescriptorSetLayoutBinding(
                    VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT, 2));

            return builder.SetVertexShader("shaders/merge.vert.spv")
                    .SetFragmentShader("shaders/merge.frag.spv")
                    .SetRasterizer(VK_POLYGON_MODE_FILL,
                                   VK_CULL_MODE_BACK_BIT,
                                   VK_FRONT_FACE_COUNTER_CLOCKWISE)
                    .AddDescriptorSetLayoutBinding(std::move(passBindings))
                    .Build();
        });

        gbufferPass->Write(normal);
        gbufferPass->Write(albedo);
        gbufferPass->Write(position);
        gbufferPass->Write(depth);

        mergePass->Read(albedo, 0);
        mergePass->Read(normal, 1);
        mergePass->Read(position, 2);
        mergePass->Read(depth);

        mergePass->Write(color);
        color->Retain();

        fg->Compile();
        fg->ResolveResource(swapChainExtent);
        fg->Execute();
    }
}

void Renderer::BuildFrameCommandBuffer(const IntrusivePtr<IView>& view,
                                       const IntrusivePtr<RenderPass>& renderPass,
                                       std::vector<IntrusivePtr<Drawable>>& drawables) {
    auto swapChain = view->GetSwapChain();
    auto swapChainExtent = swapChain->Extent();
    VkCommandBufferBeginInfo cbbi = {};
    cbbi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = renderPass->GetRenderPass();
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent.width = swapChainExtent.width;
    renderPassBeginInfo.renderArea.extent.height = swapChainExtent.height;
    renderPassBeginInfo.clearValueCount = renderPass->GetClearValue().size();
    renderPassBeginInfo.pClearValues = renderPass->GetClearValue().data();

    // for (int i = 0; i < swapChain->Size(); i++)
    //{
    //	// Set target frame buffer
    //	renderPassBeginInfo.framebuffer = frameBuffers[i];

    //	vkBeginCommandBuffer(drawCmdBuffers[i], &cbbi);

    //	vkCmdBeginRenderPass(drawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    //	VkViewport viewport = { (float)swapChainExtent.width, (float)swapChainExtent.height,
    //0.0f, 1.0f }; 	vkCmdSetViewport(drawCmdBuffers[i], 0, 1, &viewport);

    //	VkRect2D scissor = { (float)swapChainExtent.width, (float)swapChainExtent.height, 0, 0 };

    //	vkCmdSetScissor(drawCmdBuffers[i], 0, 1, &scissor);

    //	renderPass->Draw(drawCmdBuffers[i], drawables);

    //	vkCmdEndRenderPass(drawCmdBuffers[i]);

    //	vkEndCommandBuffer(drawCmdBuffers[i]);
    //}
}
