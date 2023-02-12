#pragma once

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

	void SetDepthStencil();
	void SetSwapChainImage();

private:
	friend class FrameGraph;
	IntrusivePtr<Image> image;
	IntrusivePtr<ImageView> imageView;
	VkAttachmentDescription vad;
	bool isDepthStencil = false;
	bool isSwapChainImage = false;
};
