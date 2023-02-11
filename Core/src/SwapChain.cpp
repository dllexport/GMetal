#include <SwapChain.h>

SwapChain::~SwapChain()
{
	for (auto view : swapChainImageViews)
	{
		vkDestroyImageView(context->GetVkDevice(), view->GetView(), nullptr);
	}
	swapChainImageViews.clear();
	vkDestroySwapchainKHR(context->GetVkDevice(), swapChain, nullptr);
	vkDestroySurfaceKHR(context->GetVkInstance(), surface, nullptr);
}
