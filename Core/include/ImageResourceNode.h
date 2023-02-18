#pragma once

#include <vulkan/vulkan.h>
#include <FrameGraph.h>
#include <FrameGraphNode.h>
#include <Image.h>
#include <ImageView.h>

class ImageResourceNode : public FrameGraphNode
{
public:
	ImageResourceNode(VkFormat format);
	ImageResourceNode(IntrusivePtr<Image>& image);
	virtual ~ImageResourceNode();

	void AttachmentDescriptionOverride(VkAttachmentDescription description);
	void SetLoadOp(VkAttachmentLoadOp op);
	void SetStoreOp(VkAttachmentStoreOp op);
	void SetStencilLoadOp(VkAttachmentLoadOp op);
	void SetStencilStoreOp(VkAttachmentStoreOp op);
	void SetInitLayout(VkImageLayout layout);
	void SetFinalLayout(VkImageLayout layout);
	void SetClearValue(VkClearValue clearValue);
	void SetColorBlendAttachmentState(VkPipelineColorBlendAttachmentState state);
	
	void SetDepthStencil();
	void SetSwapChainImage();

	void Resolve(VkExtent3D extend);
	
private:
	friend class FrameGraph;
	IntrusivePtr<Image> image;
	IntrusivePtr<ImageView> imageView;
	VkAttachmentDescription vad;
	bool isDepthStencil = false;
	bool isSwapChainImage = false;
	VkClearValue clearValue = {};
	VkPipelineColorBlendAttachmentState blendState = { .blendEnable = VK_FALSE };
};
