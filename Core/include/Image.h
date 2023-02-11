#pragma once

#include <unordered_set>
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

#include <IntrusivePtr.h>
#include <ImageView.h>

class Image : public IntrusiveCounter<Image>
{
public:
	void Assign(VkImage image, VkFormat format);

	VkFormat GetFormat();
private:
	friend class ImageBuilder;
	friend class ImageViewBuilder;

	VkImage image;
	VmaAllocation allocation;
	VkImageCreateInfo vici;

	std::unordered_set<IntrusivePtr<ImageView>> imageViews;
};