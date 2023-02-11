#include <ImageBuilder.h>

ImageBuilder::ImageBuilder(IntrusivePtr<VulkanContext>& context)
{
	// SetBasic
	vici.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	vici.imageType = VK_IMAGE_TYPE_2D;
	vici.extent.width = 0;
	vici.extent.height = 0;
	vici.extent.depth = 0;
	vici.format = VK_FORMAT_UNDEFINED;
	vici.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	vici.mipLevels = 1;
	vici.arrayLayers = 1;
	vici.tiling = VK_IMAGE_TILING_OPTIMAL;
	vici.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	vici.samples = VK_SAMPLE_COUNT_1_BIT;
	vici.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	vici.queueFamilyIndexCount = 0;
	vici.pQueueFamilyIndices = nullptr;
}

ImageBuilder& ImageBuilder::SetBasic(VkImageType type, VkFormat format, VkExtent3D extent, VkImageUsageFlags usage)
{
	vici.imageType = type;
	vici.format = format;
	vici.extent = extent;
	vici.usage = usage;
	return *this;
}

ImageBuilder& ImageBuilder::SetMipLevel(uint32_t level)
{
	vici.mipLevels = level;
	return *this;
}

ImageBuilder& ImageBuilder::SetArrayLayers(uint32_t size)
{
	vici.arrayLayers = size;
	return *this;
}

ImageBuilder& ImageBuilder::SetTiling(VkImageTiling tiling)
{
	vici.tiling = tiling;
	return *this;
}

ImageBuilder& ImageBuilder::SetInitialLayout(VkImageLayout layout)
{
	vici.initialLayout = layout;
	return *this;
}

ImageBuilder& ImageBuilder::SetSample(VkSampleCountFlagBits sampleFlag)
{
	vici.samples = sampleFlag;
	return *this;
}

ImageBuilder& ImageBuilder::SetAllocationInfo(VmaMemoryUsage usage, VmaAllocationCreateFlags flags)
{
	vaci = {};
	vaci.usage = VMA_MEMORY_USAGE_AUTO;
	vaci.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
	vaci.priority = 1.0f;
	return *this;
}

IntrusivePtr<Image> ImageBuilder::Build()
{
	auto image = gmetal::make_intrusive<Image>();
	vmaCreateImage(context->GetVmaAllocator(), &vici, &vaci, &image->image, &image->allocation, nullptr);
	image->vici = this->vici;
	return image;
}

