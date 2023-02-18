#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <Pipeline.h>
#include <VulkanContext.h>
#include <Drawable.h>

class RenderPass : public IntrusiveCounter<RenderPass>
{
public:
	RenderPass(IntrusivePtr<VulkanContext>& context);
	~RenderPass();

	std::vector<VkClearValue>& GetClearValue();
	VkRenderPass& GetRenderPass();
	std::vector<IntrusivePtr<Pipeline>>& GetPipelines();
	VkDescriptorPool GetDescriptorPool();
	void BuildFrameuffer(VkExtent2D extent);
	void Draw(VkCommandBuffer& commandBuffer, std::vector<IntrusivePtr<Drawable>>& drawables);

private:
	friend class RenderPassBuilder;
	friend class PipelineBuilder;
	friend class DescriptorPoolBuilder;

	IntrusivePtr<VulkanContext> context;
	VkRenderPass renderPass;

	std::vector<VkAttachmentDescription> attachments;
	std::vector<IntrusivePtr<Pipeline>> pipelines;

	// assign from DescriptorPoolBuilder
	VkDescriptorPool descriptorPool = nullptr;
	std::vector<VkDescriptorSet> descriptorSets;

	std::vector<VkClearValue> clearValues;
	std::vector<VkFramebuffer> frameBuffers;
};