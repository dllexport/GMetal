#include <FrameGraph.h>
#include <ImageBuilder.h>
#include <ImageResourceNode.h>
#include <ImageViewBuilder.h>

ImageResourceNode::ImageResourceNode(VkFormat format) {
    vad.flags = 0;
    vad.format = format;
    vad.samples = VK_SAMPLE_COUNT_1_BIT;
    vad.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    vad.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    vad.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    vad.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    vad.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    vad.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
}

ImageResourceNode::ImageResourceNode(IntrusivePtr<Image>& image) { this->image = image; }

ImageResourceNode::~ImageResourceNode() { spdlog::info("ImageResourceNode die"); }

void ImageResourceNode::SetLoadOp(VkAttachmentLoadOp op) { vad.loadOp = op; }

void ImageResourceNode::SetStoreOp(VkAttachmentStoreOp op) { vad.storeOp = op; }

void ImageResourceNode::SetStencilLoadOp(VkAttachmentLoadOp op) { vad.stencilLoadOp = op; }

void ImageResourceNode::SetStencilStoreOp(VkAttachmentStoreOp op) { vad.stencilStoreOp = op; }

void ImageResourceNode::SetInitLayout(VkImageLayout layout) { vad.initialLayout = layout; }

void ImageResourceNode::SetFinalLayout(VkImageLayout layout) { vad.finalLayout = layout; }

void ImageResourceNode::AttachmentDescriptionOverride(VkAttachmentDescription description) {
    this->vad = description;
}

void ImageResourceNode::SetClearValue(VkClearValue clearValue) { this->clearValue = clearValue; }

void ImageResourceNode::SetColorBlendAttachmentState(VkPipelineColorBlendAttachmentState state) {
    this->blendState = state;
}

void ImageResourceNode::SetDepthStencil() {
    vad.flags = 0;
    vad.samples = VK_SAMPLE_COUNT_1_BIT;
    vad.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    vad.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    vad.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    vad.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    vad.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    this->isDepthStencil = true;
    this->clearValue.depthStencil = {1.0f, 0};
}

void ImageResourceNode::SetSwapChainImage() {
    if (image) {
        vad.format = image->GetFormat();
    }
    vad.flags = 0;
    vad.samples = VK_SAMPLE_COUNT_1_BIT;
    vad.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    vad.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    vad.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    vad.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    vad.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    vad.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    this->isSwapChainImage = true;
}

void ImageResourceNode::Accept(ResourceNodeVisitor* visitor) { visitor->Visit(this); }

void ImageResourceNode::Resolve(VkExtent3D extent) {
    if (image) {
        return;
    }
    VkImageUsageFlags imageUsage = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    if (isDepthStencil) {
        imageUsage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    }

    image = ImageBuilder(this->fg->Context())
                    .SetBasic(VK_IMAGE_TYPE_2D,
                              vad.format,
                              VkExtent3D{extent.height, extent.width, 1},
                              imageUsage)
                    .Build();

    imageView = ImageViewBuilder(fg->Context())
                        .SetBasic(image,
                                  VK_IMAGE_VIEW_TYPE_2D,
                                  image->GetFormat(),
                                  VK_IMAGE_ASPECT_COLOR_BIT)
                        .Build();
}

IntrusivePtr<Image>& ImageResourceNode::GetImage() { return image; }

IntrusivePtr<ImageView>& ImageResourceNode::GetImageView() { return imageView; }
