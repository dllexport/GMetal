#pragma once

#include <vulkan/vulkan.h>

class SurfaceBuilder
{
public:
	SurfaceBuilder& SetVulkanInstance(VkInstance instance);
	SurfaceBuilder& SetNativeHandle(void* handle);
	VkSurfaceKHR Build();
private:
	VkInstance instance;
	void* nativeHandle = nullptr;
};