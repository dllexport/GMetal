#include <SwapChainBuilder.h>

#include <stdexcept>

SwapChainBuilder::SwapChainBuilder(IntrusivePtr<VulkanContext> &context, VkSurfaceKHR surface) : context(context), surface(surface)
{
}

SwapChainBuilder &SwapChainBuilder::SetPreferFormat(VkSurfaceFormatKHR format)
{
	this->preferFormat = format;
	return *this;
}

SwapChainBuilder &SwapChainBuilder::SetPreferPresentMode(VkPresentModeKHR mode)
{
	this->preferPresentMode = mode;
	return *this;
}

SwapChainBuilder &SwapChainBuilder::SetBufferCount(uint32_t count)
{
	this->bufferCount = count;
	return *this;
}

SwapChainBuilder &SwapChainBuilder::SetOldSwapChain(IntrusivePtr<SwapChain> oldSwapChain)
{
	this->oldSwapChain = oldSwapChain;
	return *this;
}

void SwapChainBuilder::BuildSwapChainProperties()
{
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context->GetVkPhysicalDevice(), surface, &capabilities);

	std::vector<VkSurfaceFormatKHR> formats;

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(context->GetVkPhysicalDevice(), surface, &formatCount, nullptr);
	if (formatCount != 0)
	{
		formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(context->GetVkPhysicalDevice(), surface, &formatCount, formats.data());
	}

	std::vector<VkPresentModeKHR> presentModes;

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(context->GetVkPhysicalDevice(), surface, &presentModeCount, nullptr);
	if (presentModeCount != 0)
	{
		presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(context->GetVkPhysicalDevice(), surface, &presentModeCount, presentModes.data());
	}

	newSwapChain->format = formats[0];
	if (std::find_if(formats.begin(), formats.end(), [&](VkSurfaceFormatKHR &format)
					 { return (format.format == preferFormat.format) && (format.colorSpace == preferFormat.colorSpace); }) != formats.end())
	{
		newSwapChain->format = preferFormat;
	}

	newSwapChain->capabilities = capabilities;

	newSwapChain->presentMode = VK_PRESENT_MODE_FIFO_KHR;
	if (std::find(presentModes.begin(), presentModes.end(), preferPresentMode) != presentModes.end())
	{
		newSwapChain->presentMode = preferPresentMode;
	}
}

void SwapChainBuilder::BuildSwapChain()
{
	VkSwapchainCreateInfoKHR sci{};
	sci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	sci.surface = surface;

	sci.minImageCount = bufferCount;
	sci.imageFormat = newSwapChain->format.format;
	sci.imageColorSpace = newSwapChain->format.colorSpace;
	sci.imageExtent = newSwapChain->capabilities.currentExtent;
	sci.imageArrayLayers = 1;
	sci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	sci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	sci.preTransform = newSwapChain->capabilities.currentTransform;
	sci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	sci.presentMode = newSwapChain->presentMode;
	sci.clipped = VK_TRUE;
	sci.oldSwapchain = VK_NULL_HANDLE;
	// oldSwapChain must keep alive until creation
	if (oldSwapChain)
	{
		sci.oldSwapchain = oldSwapChain->swapChain;
	}

	VkSwapchainKHR swapChain;
	if (vkCreateSwapchainKHR(context->GetVkDevice(), &sci, nullptr, &swapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create swap chain");
	}

	newSwapChain->swapChain = swapChain;
}

IntrusivePtr<SwapChain> SwapChainBuilder::Build()
{
	this->newSwapChain = new SwapChain();
	BuildSwapChainProperties();
	BuildSwapChain();
	this->newSwapChain->context = this->context;
	return this->newSwapChain;
}
