#pragma once

#include <vulkan/vulkan.h>

#include <VulkanContext.h>
#include <Image.h>

class ImageBuilder
{
public:
	ImageBuilder(IntrusivePtr<VulkanContext>& context);
	ImageBuilder& SetBasic(VkImageType type, VkFormat format, VkExtent3D extent, VkImageUsageFlags usage);
	ImageBuilder& SetMipLevel(uint32_t level);
	ImageBuilder& SetArrayLayers(uint32_t size);
    ImageBuilder& SetTiling(VkImageTiling tiling);
    ImageBuilder& SetInitialLayout(VkImageLayout layout);
	ImageBuilder& SetSample(VkSampleCountFlagBits sampleFlag);

	ImageBuilder& SetAllocationInfo(VmaMemoryUsage usage, VmaAllocationCreateFlags flags);
    IntrusivePtr<Image> Build();
    std::vector<IntrusivePtr<Image>> BuildVector(uint32_t size);

private:
	IntrusivePtr<VulkanContext> context;
	VkImageCreateInfo vici;
	VmaAllocationCreateInfo vaci;
};