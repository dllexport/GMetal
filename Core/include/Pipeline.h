#pragma once

#include <vulkan/vulkan.h>

#include <IntrusivePtr.h>
#include <VulkanContext.h>

class Pipeline : public IntrusiveUnsafeCounter<Pipeline>
{
public:
	Pipeline();
	~Pipeline();

	enum class Type {
		GRAPHICS,
		COMPUTE
	};

private:
	friend class PipelineBuilder;
	friend class RenderPassBuilder;
	IntrusivePtr<VulkanContext> context;
	Type type;
	VkPipeline pipeline;
	VkPipelineLayout pipelineLayouts;
	std::vector<std::vector<VkDescriptorSetLayoutBinding>> descriptorSetLayoutBindings;
	VkDescriptorSetLayout descriptorSetLayout;
};