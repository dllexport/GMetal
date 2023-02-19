#pragma once

#include <vulkan/vulkan.h>
#include <FrameGraph.h>
#include <FrameGraphNode.h>
#include <ResourceNode.h>
#include <Image.h>
#include <ImageView.h>
#include <ResourceNodeVisitor.h>

class ImageResourceNode : public ResourceNode
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

	virtual void Accept(ResourceNodeVisitor* visitor);

	virtual void Resolve(VkExtent3D extend);
	
	IntrusivePtr<Image>& GetImage();
	IntrusivePtr<ImageView>& GetImageView();
	
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
