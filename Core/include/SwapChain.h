#pragma once

#include <VulkanContext.h>
#include <vulkan/vulkan.h>

class SwapChain : public IntrusiveUnsafeCounter<SwapChain>
{
public:
    ~SwapChain();

    VkExtent2D Extent() {
        return capabilities.currentExtent;
    }

private:
    friend class SwapChainBuilder;
    IntrusivePtr<VulkanContext> context;
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR format;
    VkPresentModeKHR presentMode;
    VkSwapchainKHR swapChain;
};