#pragma once

#include <vulkan/vulkan.h>
#include <RenderPass.h>
#include <Pipeline.h>

class RenderPassBuilder
{
public:
	RenderPassBuilder(IntrusivePtr<VulkanContext>& context);
	RenderPassBuilder& SetPipelines(std::vector<IntrusivePtr<Pipeline>>&& pipelines);
	IntrusivePtr<RenderPass> Build();
private:
	IntrusivePtr<VulkanContext> context;
	IntrusivePtr<RenderPass> renderPass;
	std::vector<IntrusivePtr<Pipeline>> pipelines;

	void BuildDescriptorPool();
};