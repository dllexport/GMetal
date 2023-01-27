//
// Created by Mario on 2023/1/18.
//

#pragma once

#include <unordered_set>
#include <unordered_map>
#include <IntrusivePtr.h>
#include <VulkanContext.h>
#include <SwapChain.h>
#include <Drawable.h>
#include <RenderPass.h>

class IView : public IntrusiveUnsafeCounter<IView> {
public:
	IView(IntrusivePtr<SwapChain> swapChain);
	~IView();

	IntrusivePtr<SwapChain> GetSwapChain();
private:
	IntrusivePtr<VulkanContext> context;
	IntrusivePtr<SwapChain> swapChain;
};