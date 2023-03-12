#pragma once

#include <vulkan/vulkan.h>

#include <VulkanContext.h>
#include <Image.h>

class ImageViewBuilder
{
public:
	ImageViewBuilder(IntrusivePtr<VulkanContext>& context);
	ImageViewBuilder& SetBasic(IntrusivePtr<Image>& image, VkImageViewType type, VkFormat format, VkImageAspectFlags aspect);
	ImageViewBuilder& SetComponentMapping(VkComponentMapping mapping);
	ImageViewBuilder& SetSubresourceRange(VkImageSubresourceRange range);

	IntrusivePtr<ImageView> Build();
private:
	IntrusivePtr<VulkanContext> context;
	IntrusivePtr<Image> image;
	VkImageViewCreateInfo vivci;
};