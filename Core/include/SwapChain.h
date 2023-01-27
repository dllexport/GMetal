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

    static VkSurfaceFormatKHR SurfaceFormat() {
        return format;
    }

    static VkFormat DepthStencilFormat() {
        return depthStencilFormat;
    }

    VkSurfaceKHR GetSurface() {
        return surface;
    }

    uint32_t Size()
    {
        return this->swapChainImages.size();
    }

private:
    friend class SwapChainBuilder;
    friend class IView;
    IntrusivePtr<VulkanContext> context;
    VkSurfaceCapabilitiesKHR capabilities;
    inline static VkSurfaceFormatKHR format;
    inline static VkFormat depthStencilFormat;
    VkPresentModeKHR presentMode;

    VkSurfaceKHR surface;
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
};