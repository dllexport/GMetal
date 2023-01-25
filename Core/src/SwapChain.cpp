#include <SwapChain.h>

SwapChain::~SwapChain()
{
	vkDestroySwapchainKHR(context->GetVkDevice(), swapChain, nullptr);
}
