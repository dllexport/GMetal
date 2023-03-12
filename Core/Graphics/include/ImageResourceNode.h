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
    ImageResourceNode(std::vector<IntrusivePtr<Image>>& images);
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

	// allocate GPU resource
	virtual void Resolve(VkExtent3D extend, uint32_t size);
	
	IntrusivePtr<Image> GetImage(uint32_t index);
    IntrusivePtr<ImageView> GetImageView(uint32_t index);
	
	std::vector<IntrusivePtr<Image>>& GetImages();
    std::vector<IntrusivePtr<ImageView>>& GetImageViews();


private:
	friend class FrameGraph;
    std::vector<IntrusivePtr<Image>> images;
    std::vector<IntrusivePtr<ImageView>> imageViews;
	VkAttachmentDescription vad;
	bool isDepthStencil = false;
	bool isSwapChainImage = false;
	VkClearValue clearValue = {};
    VkPipelineColorBlendAttachmentState blendState = {
        .blendEnable = VK_FALSE,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};
};
