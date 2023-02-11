#pragma once

#include <VulkanContext.h>
#include <vulkan/vulkan.h>
#include <Image.h>
#include <ImageView.h>

class SwapChain : public IntrusiveUnsafeCounter<SwapChain>
{
public:
    ~SwapChain();

    VkExtent2D Extent() {
        return capabilities.currentExtent;
    }

    static VkSurfaceFormatKHR GetSurfaceFormat() {
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

    std::vector<IntrusivePtr<Image>>& GetImages()
    {
        return this->swapChainImages;
    }

    std::vector<IntrusivePtr<ImageView>>& GetImageViews()
    {
        return this->swapChainImageViews;
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
    std::vector<IntrusivePtr<Image>> swapChainImages;
    std::vector<IntrusivePtr<ImageView>> swapChainImageViews;
};