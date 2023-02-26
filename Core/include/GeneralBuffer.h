#pragma once

#include <unordered_set>
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

#include <IntrusivePtr.h>
#include <VulkanContext.h>

class GeneralBuffer : public IntrusiveCounter<GeneralBuffer>
{
public:
	GeneralBuffer(IntrusivePtr<VulkanContext>& context);
	~GeneralBuffer();

	VkBuffer GetBuffer() { 
		return buffer;
	}

private:
	friend class GeneralBufferBuilder;
	friend class GeneralBufferViewBuilder;
	IntrusivePtr<VulkanContext> context;
	VkBuffer buffer = nullptr;
	VmaAllocation allocation = nullptr;
};