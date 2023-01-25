//
// Created by Mario on 2023/1/18.
//

#pragma once

#include <IntrusivePtr.h>
#include <VulkanContext.h>
#include <SwapChain.h>

class IView : public IntrusiveUnsafeCounter<IView> {
public:
	IView(IntrusivePtr<SwapChain> swapChain);
	~IView();
private:
	IntrusivePtr<VulkanContext> vulkanContext;
	IntrusivePtr<SwapChain> swapChain;
};