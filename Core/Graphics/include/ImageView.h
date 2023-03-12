#pragma once

#include <vulkan/vulkan.h>
#include <VulkanContext.h>

class ImageView : public IntrusiveCounter<ImageView>
{
public:
    ImageView(IntrusivePtr<VulkanContext>& context);
    ~ImageView();
	VkImageView GetView() { return imageView; }
private:
	friend class ImageViewBuilder;
    IntrusivePtr<VulkanContext> context;
	VkImageView imageView = nullptr;
	VkImageViewCreateInfo vivci;
};