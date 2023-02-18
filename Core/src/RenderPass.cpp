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

void RenderPass::BuildFrameuffer(VkExtent2D extent)
{
	
}

void RenderPass::Draw(VkCommandBuffer &commandBuffer, std::vector<IntrusivePtr<Drawable>> &drawables)
{
	for (auto &pipeline : pipelines)
	{
	}
}
