#include <RenderPassBuilder.h>
#include <unordered_map>

RenderPassBuilder::RenderPassBuilder(IntrusivePtr<VulkanContext>& context)
{
	this->context = context;
}

RenderPassBuilder& RenderPassBuilder::SetSwapChainSize(uint32_t size) 
{ 
	this->swapChainSize = size;
	return *this;
}

RenderPassBuilder& RenderPassBuilder::SetAttachments(std::vector<VkAttachmentDescription> attachments)
{
	this->attachments = attachments;
	return *this;
}

RenderPassBuilder& RenderPassBuilder::SetClearValues(std::vector<VkClearValue> values)
{
	this->clearValues = values;
	return *this;
}

RenderPassBuilder& RenderPassBuilder::AddSubPass(VkPipelineBindPoint bindPoint, std::vector<VkAttachmentReference>&& colorRefs, std::vector<VkAttachmentReference>&& depthStencilRef, std::vector<VkAttachmentReference>&& inputRefs, std::vector<uint32_t>&& reserveRefs, VkAttachmentReference resolveRef)
{
	SubPassPair pair = {};
	pair.colorRefs = colorRefs;
	pair.depthStencilRef = depthStencilRef;
	pair.inputRefs = inputRefs;
	pair.reserveRefs = reserveRefs;
	pair.resolveRef = resolveRef;
	this->subPassPairs.push_back(pair);
	return *this;
}

RenderPassBuilder& RenderPassBuilder::AddDependency(VkSubpassDependency dependency)
{
	this->dependencies.push_back(dependency);
	return *this;
}

void RenderPassBuilder::BuildCommandBuffers()
{
	// create cmd pool
	VkCommandPoolCreateInfo cmdPoolInfo = {};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.queueFamilyIndex = context->GetQueue(VK_QUEUE_GRAPHICS_BIT).familyIndex;
    cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    vkCreateCommandPool(context->GetVkDevice(), &cmdPoolInfo, nullptr, &renderPass->commandBufferPool);

	// create cmd buffer
	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = renderPass->commandBufferPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = swapChainSize;

	renderPass->graphicCommandBuffer.resize(swapChainSize);
    vkAllocateCommandBuffers(context->GetVkDevice(),
                             &commandBufferAllocateInfo,
                             renderPass->graphicCommandBuffer.data());
}

void RenderPassBuilder::BuildSubPass()
{
	for (auto& pair : subPassPairs)
	{
		VkSubpassDescription sd = {};
		sd.pipelineBindPoint = pair.bindPoint;
		sd.colorAttachmentCount = pair.colorRefs.size();
		sd.pColorAttachments = pair.colorRefs.data();
		if (!pair.depthStencilRef.empty()) {
			sd.pDepthStencilAttachment = &pair.depthStencilRef[0];
		}
		sd.inputAttachmentCount = pair.inputRefs.size();
		sd.pInputAttachments = pair.inputRefs.data();
		sd.preserveAttachmentCount = pair.reserveRefs.size();
		sd.pPreserveAttachments = pair.reserveRefs.data();
		if (pair.resolveRef.layout == VK_IMAGE_LAYOUT_UNDEFINED) {
			sd.pResolveAttachments = nullptr;
		}
		else {
			sd.pResolveAttachments = &pair.resolveRef;
		}
		subPassDescriptions.push_back(sd);
	}

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = static_cast<uint32_t>(subPassDescriptions.size());
	renderPassInfo.pSubpasses = subPassDescriptions.data();
	renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	renderPassInfo.pDependencies = dependencies.data();

	VkRenderPass pass;
	vkCreateRenderPass(context->GetVkDevice(), &renderPassInfo, nullptr, &pass);

	renderPass->renderPass = pass;
}

IntrusivePtr<RenderPass> RenderPassBuilder::Build()
{
	this->renderPass = gmetal::make_intrusive<RenderPass>(context);
	this->renderPass->context = this->context;
	this->renderPass->clearValues = this->clearValues;
	this->renderPass->attachments = this->attachments;
	BuildSubPass();
    BuildCommandBuffers();
	return renderPass;
}