#include <ImageResourceNode.h>
#include <FrameGraph.h>

ImageResourceNode::ImageResourceNode(VkFormat format)
{
	vad.format = format;
	vad.samples = VK_SAMPLE_COUNT_1_BIT;
	vad.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	vad.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	vad.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	vad.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	vad.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	vad.finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
}

ImageResourceNode::ImageResourceNode(IntrusivePtr<Image>& image)
{
	this->image = image;
}

ImageResourceNode::~ImageResourceNode()
{

}

void ImageResourceNode::SetLoadOp(VkAttachmentLoadOp op)
{
	vad.loadOp = op;
}

void ImageResourceNode::SetStoreOp(VkAttachmentStoreOp op)
{
	vad.storeOp = op;
}

void ImageResourceNode::SetStencilLoadOp(VkAttachmentLoadOp op)
{
	vad.stencilLoadOp = op;
}

void ImageResourceNode::SetStencilStoreOp(VkAttachmentStoreOp op)
{
	vad.stencilStoreOp = op;
}

void ImageResourceNode::SetInitLayout(VkImageLayout layout)
{
	vad.initialLayout = layout;
}

void ImageResourceNode::SetFinalLayout(VkImageLayout layout)
{
	vad.finalLayout = layout;
}

void ImageResourceNode::AttachmentDescriptionOverride(VkAttachmentDescription description)
{
	this->vad = description;
}


void ImageResourceNode::SetDepthStencil()
{
	vad.samples = VK_SAMPLE_COUNT_1_BIT;
	vad.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	vad.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	vad.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	vad.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
}

void ImageResourceNode::SetColor()
{
	if (image) {
		vad.format = image->GetFormat();
	}
	vad.samples = VK_SAMPLE_COUNT_1_BIT;
	vad.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	vad.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	vad.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	vad.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	vad.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	vad.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
}