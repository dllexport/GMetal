//
// Created by Mario on 2023/1/18.
//
#pragma once

#include <unordered_map>

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

    VkInstance& GetVkInstance() {
        return instance;
    }

    VkPhysicalDevice& GetVkPhysicalDevice() {
        return physicalDevice;
    }

    VkDevice& GetVkDevice() {
        return logicalDevice;
    }

    VmaAllocator& GetVmaAllocator() {
        return vmaAllocator;
    }

private:
    friend class VulkanContextBuilder;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;

    VkPhysicalDevice physicalDevice;
    VkDevice logicalDevice;

    // assume graphics and present are in same queue
    std::unordered_map<VkQueueFlagBits, DeviceQueueContext> queueContextMap;

    VmaAllocator vmaAllocator;

    std::vector<VkExtensionProperties> enabledInstanceExtensions;
    std::vector<VkLayerProperties> enabledInstanceLayers;
};
