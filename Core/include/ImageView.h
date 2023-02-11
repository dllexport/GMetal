#pragma once

#include <vulkan/vulkan.h>
#include <Image.h>

class ImageView : public IntrusiveCounter<ImageView>
{
public:
	VkImageView GetView() { return imageView; }
private:
	friend class ImageViewBuilder;
	friend class Image;
	VkImageView imageView;
	VkImageViewCreateInfo vivci;
};