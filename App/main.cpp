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
#include <RenderPassBuilder.h>
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


    std::vector<VkAttachmentDescription> attachments;
    attachments.resize(2);
    attachments[0].format = swapChain1->SurfaceFormat().format;                     // Use the color format selected by the swapchain
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;                                 // We don't use multi sampling in this example
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;                            // Clear this attachment at the start of the render pass
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;                          // Keep its contents after the render pass is finished (for displaying it)
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;                 // We don't use stencil, so don't care for load
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;               // Same for store
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;                       // Layout at render pass start. Initial doesn't matter, so we use undefined
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;                   // Layout to which the attachment is transitioned when the render pass is finished

    // attachments[1].format = depthFormat;                                         // A proper depth format is selected in the example base
    attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;                            // Clear depth at start of first subpass
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;                      // We don't need depth after render pass has finished (DONT_CARE may result in better performance)
    attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;                 // No stencil
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;               // No Stencil
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;                       // Layout at render pass start. Initial doesn't matter, so we use undefined
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;  // Transition to depth/stencil attachment

    auto renderPass = RenderPassBuilder(vkContext)
        .SetAttachments(attachments)
        .AddSubPass(VK_PIPELINE_BIND_POINT_GRAPHICS, { {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL} }, { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL })
        .Build();

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