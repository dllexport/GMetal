#pragma once

#include <unordered_set>
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

#include <IntrusivePtr.h>
#include <ImageView.h>
#include <VulkanContext.h>

class Image : public IntrusiveCounter<Image>
{
public:
	Image(IntrusivePtr<VulkanContext>& context);
	~Image();

	void Assign(VkImage image, VkFormat format);
	VkFormat GetFormat();

private:
	friend class ImageBuilder;
	friend class ImageViewBuilder;
	IntrusivePtr<VulkanContext> context;
	VkImage image = nullptr;
	VmaAllocation allocation = nullptr;
	VkImageCreateInfo vici = {};
	bool externResource = false;
	std::unordered_set<IntrusivePtr<ImageView>> imageViews;
};