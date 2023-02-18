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

IntrusivePtr<RenderPass> BuildDefaultRenderPass(IntrusivePtr<VulkanContext> context, IntrusivePtr<SwapChain> swapchain)
{
    std::vector<VkAttachmentDescription> attachments(2);
    attachments[0].format = swapchain->GetSurfaceFormat().format;                     // Use the color format selected by the swapchain
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;                                 // We don't use multi sampling in this example
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;                            // Clear this attachment at the start of the render pass
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;                          // Keep its contents after the render pass is finished (for displaying it)
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;                 // We don't use stencil, so don't care for load
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;               // Same for store
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;                       // Layout at render pass start. Initial doesn't matter, so we use undefined
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;                   // Layout to which the attachment is transitioned when the render pass is finished

    attachments[1].format = swapchain->DepthStencilFormat();                       // A proper depth format is selected in the example base
    attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;                            // Clear depth at start of first subpass
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;                      // We don't need depth after render pass has finished (DONT_CARE may result in better performance)
    attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;                 // No stencil
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;               // No Stencil
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;                       // Layout at render pass start. Initial doesn't matter, so we use undefined
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;  // Transition to depth/stencil attachment

    std::vector<VkSubpassDependency> dependencies(2);
    // Does the transition from final to initial layout for the depth an color attachments
    // Depth attachment
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    // Color attachment
    dependencies[1].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].dstSubpass = 0;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].srcAccessMask = 0;
    dependencies[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    std::vector<VkClearValue> clearValues(2);
    clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 0.0f } };
    clearValues[1].depthStencil = { 1.0f, 0 };

    return nullptr;
    /*auto renderPass = RenderPassBuilder(context)
        .SetAttachments(attachments)
        .SetClearValues(clearValues)
        .AddSubPass(VK_PIPELINE_BIND_POINT_GRAPHICS, { {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL} }, { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL })
        .AddDependency(dependencies[0])
        .AddDependency(dependencies[1])
        .Build();
     
    auto pipeline = PipelineBuilder(context)
        .SetRenderPass(renderPass, 0)
        .SetVertexShader("shaders/test.vert.spv")
        .SetFragmentShader("shaders/test.frag.spv")
        .SetVertexInput({ VertexComponent::Position, VertexComponent::Color })
        .SetVertexAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .SetRasterizer(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE)
        .AddDescriptorSetLayoutBinding({ PipelineBuilder::BuildDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0) })
        .Build();

    DescriptorPoolBuilder(context).SetRenderPass(renderPass).AddPipelines(pipeline).Build();

    return renderPass;*/
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