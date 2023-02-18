#pragma once

#include <vulkan/vulkan.h>
#include <VulkanContext.h>
#include <Pipeline.h>
#include <RenderPass.h>

class DescriptorPoolBuilder
{
public:
	DescriptorPoolBuilder(IntrusivePtr<VulkanContext>& context);
	DescriptorPoolBuilder& AddPipeline(IntrusivePtr<Pipeline>& pipeline);
	DescriptorPoolBuilder& SetPipelines(std::vector<IntrusivePtr<Pipeline>> pipelines);
	DescriptorPoolBuilder& SetRenderPass(IntrusivePtr<RenderPass>& renderPass);
	void Build();
private:
	IntrusivePtr<VulkanContext> context;
	std::vector<IntrusivePtr<Pipeline>> pipelines;
	IntrusivePtr<RenderPass> renderPass;

	void BuildDescriptorPool();
};