//
// Created by Mario on 2023/1/18.
//

#pragma once

#include <IntrusivePtr.h>
#include <VulkanContext.h>
#include <SwapChain.h>
#include <vulkan/vulkan.h>

class IView : public IntrusiveUnsafeCounter<IView> {
public:
	IView(boost::intrusive_ptr<VulkanContext>& context, VkSurfaceKHR surface);
	~IView();
protected:
	boost::intrusive_ptr<VulkanContext> vulkanContext;
	VkSurfaceKHR surface;
};