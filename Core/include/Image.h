#pragma once

#include <vulkan/vulkan.h>

class Image
{
private:
	friend class ImageBuilder;
	VkImage image = VK_NULL_HANDLE;
	VkDeviceMemory mem = VK_NULL_HANDLE;
	VkImageView view = VK_NULL_HANDLE;
	VkFormat format;
};