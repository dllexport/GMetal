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

    VkSurfaceKHR GetSurface() {
        return surface;
    }

private:
    friend class SwapChainBuilder;
    friend class IView;
    IntrusivePtr<VulkanContext> context;
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR format;
    VkPresentModeKHR presentMode;

    VkSurfaceKHR surface;
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
};