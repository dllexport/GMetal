//
// Created by Mario on 2023/1/18.
//

#include <VulkanContext.h>

VulkanContext::~VulkanContext()
{
    vmaDestroyAllocator(vmaAllocator);
    vkDestroyDevice(device, nullptr);
    physicalDevice = nullptr;

    if (debugMessenger) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        func(instance, debugMessenger, nullptr);
    }

    vkDestroyInstance(instance, nullptr);
}
