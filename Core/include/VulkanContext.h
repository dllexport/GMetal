//
// Created by Mario on 2023/1/18.
//
#pragma once

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>
#include <IntrusivePtr.h>

class VulkanContext : public IntrusiveUnsafeCounter<VulkanContext> {
public:
    VulkanContext() = default;
    ~VulkanContext();
private:
    friend class VulkanContextBuilder;
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VmaAllocator vmaAllocator;
};
