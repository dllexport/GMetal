//
// Created by Mario on 2023/1/18.
//

#include <VulkanContext.h>

VulkanContext::~VulkanContext()
{
    vmaDestroyAllocator(vmaAllocator);
    vkDestroyDevice(device, nullptr);
    physicalDevice = nullptr;
    vkDestroyInstance(instance, nullptr);
}
