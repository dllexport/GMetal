#ifdef __APPLE__

#include <SurfaceBuilder.h>
#include <stdexcept>
#include <GLFW/glfw3.h>

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
    VkSurfaceKHR surface;
    VkResult err = glfwCreateWindowSurface(instance, (GLFWwindow *)this->nativeHandle, NULL, &surface);
    if (err != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface");
	}

    return surface;
}

#endif
