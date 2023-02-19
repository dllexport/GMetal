#include <SwapChain.h>

SwapChain::~SwapChain()
{
	vkDestroySwapchainKHR(context->GetVkDevice(), swapChain, nullptr);
	vkDestroySurfaceKHR(context->GetVkInstance(), surface, nullptr);
}
