#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <Pipeline.h>
#include <IntrusivePtr.h>

class RenderPass : public IntrusiveCounter<RenderPass>
{
public:
	RenderPass() = default;
	~RenderPass();
private:
	friend class RenderPassBuilder;
	friend class PipelineBuilder;
	friend class DescriptorPoolBuilder;

	IntrusivePtr<VulkanContext> context;
	std::vector<VkAttachmentDescription> attachments;
	std::vector<IntrusivePtr<Pipeline>> pipelines;
	VkRenderPass renderPass;

	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;
};