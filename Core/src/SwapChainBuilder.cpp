#include <SwapChainBuilder.h>

#include <stdexcept>
#include <spdlog/spdlog.h>
#include <ImageViewBuilder.h>

// for vk string convertion
#ifndef NDEBUG
#include <vulkan/vulkan.hpp>
#endif

SwapChainBuilder::SwapChainBuilder(IntrusivePtr<VulkanContext> &context, VkSurfaceKHR surface) : context(context), surface(surface)
{
}

SwapChainBuilder &SwapChainBuilder::SetPreferFormat(VkSurfaceFormatKHR format)
{
	this->preferFormat = format;
	return *this;
}

SwapChainBuilder& SwapChainBuilder::SetPreferDepthStencilFormat(VkFormat format)
{
	this->preferDepthStencilFormat = format;
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

#ifndef NDEBUG
	static std::once_flag flag = {};
	std::call_once(flag, [&]() {
		spdlog::debug("available swapchain format:");
		for (auto& format : formats)
		{
			spdlog::debug("{}:{}", vk::to_string(vk::Format(format.format)), vk::to_string(vk::ColorSpaceKHR(format.colorSpace)));
		}

		spdlog::debug("available present mode:");
		for (auto& mode : presentModes)
		{
			spdlog::debug("{}", vk::to_string(vk::PresentModeKHR(mode)));
		} 
	});
#endif
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
	uint32_t imageCount;
	vkGetSwapchainImagesKHR(context->GetVkDevice(), swapChain, &imageCount, nullptr);

	std::vector<VkImage> images;
	images.resize(imageCount);
	vkGetSwapchainImagesKHR(context->GetVkDevice(), swapChain, &imageCount, images.data());
	newSwapChain->swapChainImages.resize(imageCount);
	newSwapChain->swapChainImageViews.resize(imageCount);
	for (int i = 0; i < imageCount; i++)
	{
		auto image = gmetal::make_intrusive<Image>(context);
		image->Assign(images[i], newSwapChain->format.format);
		auto imageView = ImageViewBuilder(context).SetBasic(image, VK_IMAGE_VIEW_TYPE_2D, newSwapChain->format.format, VK_IMAGE_ASPECT_COLOR_BIT).Build();
		newSwapChain->swapChainImages[i] = image;
		newSwapChain->swapChainImageViews[i] = imageView;
	}
}

void SwapChainBuilder::ResolveDepthStencilFormat()
{
	// Since all depth formats may be optional, we need to find a suitable depth format to use
	// Start with the highest precision packed format
	std::vector<VkFormat> depthFormats = {
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM
	};

	if (this->preferDepthStencilFormat != VK_FORMAT_UNDEFINED) {
		depthFormats.insert(depthFormats.begin(), preferDepthStencilFormat);
	}

	for (auto& format : depthFormats)
	{
		VkFormatProperties formatProps;
		vkGetPhysicalDeviceFormatProperties(context->GetVkPhysicalDevice(), format, &formatProps);
		// Format must support depth stencil attachment for optimal tiling
		if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			newSwapChain->depthStencilFormat = format;
			break;
		}
	}
}

IntrusivePtr<SwapChain> SwapChainBuilder::Build()
{
	this->newSwapChain = new SwapChain();
	newSwapChain->context = this->context;
	BuildSwapChainProperties();
	BuildSwapChain();
	ResolveDepthStencilFormat();
	newSwapChain->surface = this->surface;
	return this->newSwapChain;
}
