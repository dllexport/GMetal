#pragma once

#include <vulkan/vulkan.h>
#include <SwapChain.h>
#include <VulkanContext.h>

class SwapChainBuilder
{
public:
	SwapChainBuilder(IntrusivePtr<VulkanContext>& context, VkSurfaceKHR surface);
	SwapChainBuilder& SetPreferFormat(VkSurfaceFormatKHR format);
	SwapChainBuilder& SetPreferPresentMode(VkPresentModeKHR mode);
	SwapChainBuilder& SetBufferCount(uint32_t count);
	SwapChainBuilder& SetOldSwapChain(IntrusivePtr<SwapChain> oldSwapChain);
	IntrusivePtr<SwapChain> Build();
private:
	IntrusivePtr<SwapChain> newSwapChain;
	IntrusivePtr<SwapChain> oldSwapChain;
	IntrusivePtr<VulkanContext> context;
	VkSurfaceKHR surface;
	VkSurfaceFormatKHR preferFormat;
	VkPresentModeKHR preferPresentMode;
	uint32_t bufferCount = 2;

	void BuildSwapChainProperties();
	void BuildSwapChain();
};