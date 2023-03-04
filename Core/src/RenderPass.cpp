#include <RenderPass.h>
#include <RenderPassBuilder.h>
#include <SwapChain.h>
#include <ImageBuilder.h>
#include <FrameGraph.h>

RenderPass::RenderPass(IntrusivePtr<VulkanContext> &context) : context(context)
{
}

RenderPass::~RenderPass()
{
    vkDestroyCommandPool(context->GetVkDevice(), this->commandBufferPool, nullptr);

    for (auto frameBuffer : this->frameBuffers) {
        vkDestroyFramebuffer(context->GetVkDevice(), frameBuffer, nullptr);
    }
	vkDestroyDescriptorPool(context->GetVkDevice(), this->descriptorPool, nullptr);
	vkDestroyRenderPass(context->GetVkDevice(), this->renderPass, nullptr);
}

std::vector<VkClearValue> &RenderPass::GetClearValue()
{
	return this->clearValues;
}

VkRenderPass &RenderPass::GetRenderPass()
{
	return this->renderPass;
}

std::vector<IntrusivePtr<Pipeline>> &RenderPass::GetPipelines()
{
	return this->pipelines;
}

VkDescriptorPool RenderPass::GetDescriptorPool()
{
	return this->descriptorPool;
}

void RenderPass::BuildFrameuffer(std::vector<VkImageView> attachments, VkExtent2D extent) {
    VkFramebufferCreateInfo frameBufferCreateInfo = {};
    frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferCreateInfo.renderPass = renderPass;
    frameBufferCreateInfo.attachmentCount = attachments.size();
    frameBufferCreateInfo.pAttachments = attachments.data();
    frameBufferCreateInfo.width = extent.width;
    frameBufferCreateInfo.height = extent.height;
    frameBufferCreateInfo.layers = 1;

	VkFramebuffer frameBuffer;
    vkCreateFramebuffer(context->GetVkDevice(), &frameBufferCreateInfo, nullptr, &frameBuffer);
    this->frameBuffers.push_back(frameBuffer);
}

void RenderPass::Draw(VkCommandBuffer &commandBuffer, std::vector<IntrusivePtr<Drawable>> &drawables)
{
	for (auto &pipeline : pipelines)
	{
	}
}
