#include <RenderPass.h>
#include <RenderPassBuilder.h>
#include <SwapChain.h>
#include <ImageBuilder.h>

RenderPass::~RenderPass()
{
	vkDestroyDescriptorPool(context->GetVkDevice(), this->descriptorPool, nullptr);
	vkDestroyRenderPass(context->GetVkDevice(), this->renderPass, nullptr);
}

std::vector<VkClearValue>& RenderPass::GetClearValue()
{
	return this->clearValues;
}

VkRenderPass RenderPass::GetRenderPass()
{
	return this->renderPass;
}

std::vector<IntrusivePtr<Pipeline>>& RenderPass::GetPipelines()
{
	return this->pipelines;
}

void RenderPass::BuildFrameuffer(VkExtent2D extent)
{
}

void RenderPass::Draw(VkCommandBuffer& commandBuffer, std::vector<IntrusivePtr<Drawable>>& drawables)
{
	for (auto& pipeline : pipelines)
	{

	}
}
