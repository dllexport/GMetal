#pragma once

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

class Image
{
public:
	VkImageView CreateView();
private:
	friend class ImageBuilder;
	VkImage image;
	VmaAllocation allocation;
	VkImageCreateInfo vici;
};