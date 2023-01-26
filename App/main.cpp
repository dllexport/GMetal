//
// Created by Mario on 2023/1/18.
//
#include <iostream>
#include <VulkanContext.h>
#include <VulkanContextBuilder.h>
#include <spdlog/spdlog.h>
#include <SurfaceBuilder.h>
#include <IView.h>
#include <SwapChainBuilder.h>
#include <PipelineBuilder.h>
#include <Vertex.h>
#include <Windows.h>
#include <WinUser.h>


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

HWND CreateWin32Window()
{
    LPCSTR CLASS_NAME = {"Sample Window Class"};

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,                            // Optional window styles.
        CLASS_NAME,                   // Window class
        {"Learn to Program Windows"}, // Window text
        WS_OVERLAPPEDWINDOW,          // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,         // Parent window
        NULL,         // Menu
        wc.hInstance, // Instance handle
        NULL          // Additional application data
    );

    if (!hwnd)
    {
        std::runtime_error("Failed to create window");
    }
    else
    {
        ShowWindow(hwnd, SW_NORMAL);
    }

    return hwnd;
}

int main()
{
    spdlog::set_level(spdlog::level::info);

    auto handle1 = CreateWin32Window();
    auto handle2 = CreateWin32Window();

    auto vkContext =
        VulkanContextBuilder()
            .SetInstanceExtensions({VK_KHR_SURFACE_EXTENSION_NAME, "VK_KHR_win32_surface"})
            .EnableValidationLayer()
            .SetInstanceLayers({"VK_LAYER_KHRONOS_validation"})
            .SetDeviceExtensions({VK_KHR_SWAPCHAIN_EXTENSION_NAME})
            .Build();

    auto surface1 = SurfaceBuilder().SetVulkanInstance(vkContext->GetVkInstance()).SetNativeHandle(handle1).Build();
    auto surface2 = SurfaceBuilder().SetVulkanInstance(vkContext->GetVkInstance()).SetNativeHandle(handle2).Build();

    auto swapChain1 = SwapChainBuilder(vkContext, surface1).SetPreferFormat({VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}).Build();
    auto swapChain2 = SwapChainBuilder(vkContext, surface2).SetPreferFormat({VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}).Build();

    auto view1 = gmetal::make_intrusive<IView>(swapChain1);
    auto view2 = gmetal::make_intrusive<IView>(swapChain2);

    auto pipeline = PipelineBuilder(vkContext)
        .SetVertexShader("shaders/test.vert.spv")
        .SetFragmentShader("shaders/test.frag.spv")
        .SetVertexInput({ VertexComponent::Position, VertexComponent::Color, VertexComponent::Normal, VertexComponent::UV })
        .SetVertexAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .SetRasterizer(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE)
        .AddDescriptorSetLayoutBinding({ PipelineBuilder::BuildDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0) })
        .Build();

    return 0;
}