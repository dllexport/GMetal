#ifdef __APPLE__
//
// Created by Mario on 2023/1/18.
//
#include <iostream>
#include <VulkanContext.h>
#include <spdlog/spdlog.h>
#include <IView.h>

#include <VulkanContextBuilder.h>
#include <SurfaceBuilder.h>
#include <SwapChainBuilder.h>
#include <PipelineBuilder.h>
#include <RenderPassBuilder.h>
#include <DescriptorPoolBuilder.h>

#include <Renderer.h>
#include <Drawable.h>

#include <Vertex.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>


void* CreateWindow()
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(640, 480, "Window Title", NULL, NULL);
    return window;
}

int main()
{
    spdlog::set_level(spdlog::level::info);

    glfwInit();

    if (!glfwVulkanSupported())
    {
        throw std::runtime_error("!glfwVulkanSupported()");
    }

uint32_t count;
const char** extensions = glfwGetRequiredInstanceExtensions(&count);
const char** extensions2 = extensions + 1;
    auto handle1 = CreateWindow();

    auto vkContext =
        VulkanContextBuilder()
            .SetInstanceExtensions({VK_KHR_SURFACE_EXTENSION_NAME, "VK_EXT_metal_surface"})
            .EnableValidationLayer()
            .SetInstanceLayers({"VK_LAYER_KHRONOS_validation"})
            .SetDeviceExtensions({VK_KHR_SWAPCHAIN_EXTENSION_NAME})
            .Build();

    auto surface1 = SurfaceBuilder().SetVulkanInstance(vkContext->GetVkInstance()).SetNativeHandle(handle1).Build();
    auto swapChain1 = SwapChainBuilder(vkContext, surface1).SetPreferFormat({VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}).Build();
    auto view1 = gmetal::make_intrusive<IView>(swapChain1);

    auto drawable = gmetal::make_intrusive<Drawable>();
    drawable->SetVertexArray({});
    drawable->SetVertexTopology(Drawable::DrawTopology::TRIANGLE_LIST, Drawable::UINT16);

    auto renderer = gmetal::make_intrusive<Renderer>(vkContext);
    renderer->AddView(view1);
    renderer->AddDrawable(drawable, view1);

    renderer->StartFrame();

    return 0;
}

#endif