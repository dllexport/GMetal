#pragma once

#include <vulkan/vulkan.h>

#include <IntrusivePtr.h>
#include <VulkanContext.h>
#include <Drawable.h>

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
	friend class FrameGraph;
	friend class PipelineBuilder;
	friend class RenderPassBuilder;
	friend class DescriptorPoolBuilder;
	friend class Renderer;

	IntrusivePtr<VulkanContext> context;
	Type type;
	VkPipeline pipeline;
	std::vector<VkShaderModule> shaderModules;
	VkPipelineLayout pipelineLayout;
	// pipeline could have multiple descriptor set
	std::vector<std::vector<VkDescriptorSetLayoutBinding>> descriptorSetLayoutBindings;
	std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
	VkDescriptorSet descriptorSet;
};