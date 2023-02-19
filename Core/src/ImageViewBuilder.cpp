#include <ImageViewBuilder.h>

ImageViewBuilder::ImageViewBuilder(IntrusivePtr<VulkanContext>& context) : context(context)
{
	vivci = {};
	vivci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	vivci.components = {};
}

ImageViewBuilder& ImageViewBuilder::SetBasic(IntrusivePtr<Image>& image, VkImageViewType type, VkFormat format, VkImageAspectFlags aspect)
{
	this->image = image;
	vivci.viewType = type;
	vivci.image = image->image;
	vivci.format = format;
	vivci.subresourceRange.aspectMask = aspect;
	vivci.subresourceRange.baseMipLevel = 0;
	vivci.subresourceRange.levelCount = 1;
	vivci.subresourceRange.baseArrayLayer = 0;
	vivci.subresourceRange.layerCount = 1;
	return *this;
}

ImageViewBuilder& ImageViewBuilder::SetComponentMapping(VkComponentMapping mapping)
{
	vivci.components = mapping;
	return *this;
}

ImageViewBuilder& ImageViewBuilder::SetSubresourceRange(VkImageSubresourceRange range)
{
	vivci.subresourceRange = range;
	return *this;
}

IntrusivePtr<ImageView> ImageViewBuilder::Build()
{
	auto imageView = gmetal::make_intrusive<ImageView>(context);
	vkCreateImageView(context->GetVkDevice(), &vivci, nullptr, &imageView->imageView);
	imageView->vivci = this->vivci;
	this->image->imageViews.insert(imageView);
	return imageView;
}