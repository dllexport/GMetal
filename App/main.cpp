#ifdef _WIN32
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
    // auto handle2 = CreateWin32Window();

    auto vkContext =
        VulkanContextBuilder()
            .SetInstanceExtensions({VK_KHR_SURFACE_EXTENSION_NAME, "VK_KHR_win32_surface"})
            .EnableValidationLayer()
            .SetInstanceLayers({"VK_LAYER_KHRONOS_validation"})
            .SetDeviceExtensions({VK_KHR_SWAPCHAIN_EXTENSION_NAME})
            .Build();

    auto surface1 = SurfaceBuilder().SetVulkanInstance(vkContext->GetVkInstance()).SetNativeHandle(handle1).Build();
    // auto surface2 = SurfaceBuilder().SetVulkanInstance(vkContext->GetVkInstance()).SetNativeHandle(handle2).Build();

    auto swapChain1 = SwapChainBuilder(vkContext, surface1).SetPreferFormat({VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}).Build();
    // auto swapChain2 = SwapChainBuilder(vkContext, surface2).SetPreferFormat({VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}).Build();

    auto view1 = gmetal::make_intrusive<IView>(swapChain1);
    // auto view2 = gmetal::make_intrusive<IView>(swapChain2);

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