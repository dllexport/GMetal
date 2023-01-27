#pragma once

#include <vulkan/vulkan.h>

#include <VulkanContext.h>
#include <Image.h>

class ImageBuilder
{
public:
	ImageBuilder(IntrusivePtr<VulkanContext>& context);
	ImageBuilder& SetFormat(VkFormat format);
	ImageBuilder& SetUsage(VkImageUsageFlags usage);
private:
	IntrusivePtr<VulkanContext> context;
};