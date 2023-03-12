//
// Created by Mario on 2023/1/18.
//

#pragma once

#include <IntrusivePtr.h>
#include <VulkanContext.h>
#include <SwapChain.h>
#include <Drawable.h>
#include <RenderPass.h>
#include <Renderer.h>

class IView : public IntrusiveUnsafeCounter<IView> {
public:
    IView(IntrusivePtr<VulkanContext> context);
	~IView();

	IntrusivePtr<SwapChain> GetSwapChain();
    void SetSwapChain(IntrusivePtr<SwapChain> swapChain);

private:
	IntrusivePtr<VulkanContext> context;
	IntrusivePtr<SwapChain> swapChain;
};