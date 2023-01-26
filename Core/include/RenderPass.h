#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <Pipeline.h>
#include <IntrusivePtr.h>

class RenderPass : public IntrusiveCounter<RenderPass>
{
public:

private:
	friend class RenderPassBuilder;
	IntrusivePtr<VulkanContext> context;
	std::vector<VkAttachmentDescription> attachments;
	std::vector<IntrusivePtr<Pipeline>> pipelines;
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;
};