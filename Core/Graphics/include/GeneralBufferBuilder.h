#pragma once

#include <vulkan/vulkan.h>

#include <GeneralBuffer.h>
#include <VulkanContext.h>

class GeneralBufferBuilder {
public:
    GeneralBufferBuilder(IntrusivePtr<VulkanContext>& context);
    GeneralBufferBuilder& SetBasic(VkDeviceSize size, VkBufferUsageFlags usage);
    GeneralBufferBuilder& SetSharingMode(VkSharingMode mode);
    GeneralBufferBuilder& SetQueueFamilies(std::vector<uint32_t> queueFamilies);

    GeneralBufferBuilder& SetAllocationInfo(VmaMemoryUsage usage, VmaAllocationCreateFlags flags);

    IntrusivePtr<GeneralBuffer> Build();

private:
    IntrusivePtr<VulkanContext> context;
    VkBufferCreateInfo vbci;
    VmaAllocationCreateInfo vaci;
    std::vector<uint32_t> queueFamilies;
};