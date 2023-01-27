#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <Pipeline.h>
#include <IntrusivePtr.h>
#include <Drawable.h>

class RenderPass : public IntrusiveCounter<RenderPass>
{
public:
	RenderPass() = default;
	~RenderPass();

	std::vector<VkClearValue>& GetClearValue();
	VkRenderPass GetRenderPass();
	std::vector<IntrusivePtr<Pipeline>>& GetPipelines();

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

	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

	std::vector<VkClearValue> clearValues;
	std::vector<VkFramebuffer> frameBuffers;
};