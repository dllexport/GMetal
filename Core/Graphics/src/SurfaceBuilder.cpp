#ifdef _WIN32

#include <SurfaceBuilder.h>
#include <stdexcept>

#include <Windows.h>
#include <vulkan/vulkan_win32.h>

SurfaceBuilder &SurfaceBuilder::SetVulkanInstance(VkInstance instance)
{
	this->instance = instance;
	return *this;
}

SurfaceBuilder &SurfaceBuilder::SetNativeHandle(void *handle)
{
	this->nativeHandle = handle;
	return *this;
}

VkSurfaceKHR SurfaceBuilder::Build()
{
	VkWin32SurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = reinterpret_cast<HWND>(this->nativeHandle);
	createInfo.hinstance = GetModuleHandle(nullptr);

	if (!nativeHandle)
	{
		throw std::runtime_error("Null surface handle");
	}

	VkSurfaceKHR surface;
	if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface");
	}
	return surface;
}

#endif
