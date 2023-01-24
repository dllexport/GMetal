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

    struct DeviceQueueContext {
        VkQueue queue;
        uint32_t familyIndex;
    };

private:
    friend class VulkanContextBuilder;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;

    VkPhysicalDevice physicalDevice;
    VkDevice logicalDevice;

    DeviceQueueContext graphicsQueue;
    DeviceQueueContext computeQueue;
    DeviceQueueContext transferQueue;

    VmaAllocator vmaAllocator;

    std::vector<VkExtensionProperties> enabledInstanceExtensions;
    std::vector<VkLayerProperties> enabledInstanceLayers;
};
