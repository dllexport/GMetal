#pragma once

#include <vulkan/vulkan.h>
#include <RenderPass.h>
#include <Pipeline.h>

class RenderPassBuilder
{
public:
	RenderPassBuilder(IntrusivePtr<VulkanContext>& context);
	RenderPassBuilder& SetAttachments(std::vector<VkAttachmentDescription> attachments);
	RenderPassBuilder& SetClearValues(std::vector<VkClearValue> values);
	RenderPassBuilder& AddSubPass(VkPipelineBindPoint bindPoint, std::vector<VkAttachmentReference>&& colorRefs, VkAttachmentReference depthStencilRef, std::vector<VkAttachmentReference>&& inputRefs = {}, std::vector<uint32_t>&& reserveRefs = {}, VkAttachmentReference resolveRef = {});
	RenderPassBuilder& AddDependency(VkSubpassDependency dependency);
	IntrusivePtr<RenderPass> Build();

private:
	IntrusivePtr<VulkanContext> context;
	IntrusivePtr<RenderPass> renderPass;

	struct SubPassPair
	{
		std::vector<VkAttachmentReference> inputRefs;
		std::vector<VkAttachmentReference> colorRefs;
		std::vector<uint32_t> reserveRefs;
		VkAttachmentReference resolveRef;
		VkAttachmentReference depthStencilRef;
		VkPipelineBindPoint bindPoint;
	};
	
	std::vector<VkAttachmentDescription> attachments;
	std::vector<VkClearValue> clearValues;

	std::vector<SubPassPair> subPassPairs;
	std::vector<VkSubpassDescription> subPassDescriptions;
	std::vector<VkSubpassDependency> dependencies;

	void BuildSubPass();
};