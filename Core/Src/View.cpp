#include <View.h>

#include <IView.h>

#include <stdexcept>

#include <VulkanContextBuilder.h>
#include <SurfaceBuilder.h>
#include <SwapChainBuilder.h>
#include <IView.h>

#include <Windows.h>
#include <WinUser.h>

auto vkContext =
        VulkanContextBuilder()
                .SetInstanceExtensions({VK_KHR_SURFACE_EXTENSION_NAME, "VK_KHR_win32_surface"})
                .EnableValidationLayer()
                .SetInstanceLayers({"VK_LAYER_KHRONOS_validation"})
                .SetDeviceExtensions({VK_KHR_SWAPCHAIN_EXTENSION_NAME})
                .Build();

auto renderer = gmetal::make_intrusive<Renderer>(vkContext);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

HWND CreateWin32Window() {
    LPCSTR CLASS_NAME = {"Sample Window Class"};

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0,                             // Optional window styles.
                               CLASS_NAME,                    // Window class
                               {"Learn to Program Windows"},  // Window text
                               WS_OVERLAPPEDWINDOW,           // Window style

                               // Size and position
                               CW_USEDEFAULT,
                               CW_USEDEFAULT,
                               CW_USEDEFAULT,
                               CW_USEDEFAULT,

                               NULL,          // Parent window
                               NULL,          // Menu
                               wc.hInstance,  // Instance handle
                               NULL           // Additional application data
    );

    if (!hwnd) {
        std::runtime_error("Failed to create window");
    } else {
        ShowWindow(hwnd, SW_NORMAL);
    }

    return hwnd;
}

View::View() {
    this->graphicView = gmetal::make_intrusive<IView>(vkContext);

    auto handle = CreateWin32Window();

    auto surface = SurfaceBuilder()
                            .SetVulkanInstance(vkContext->GetVkInstance())
                            .SetNativeHandle(handle)
                            .Build();

    auto swapChain =
            SwapChainBuilder(vkContext, surface)
                    .SetPreferFormat({VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
                    .Build();
    auto view = gmetal::make_intrusive<IView>(vkContext);
    view->SetSwapChain(swapChain);

    renderer->AddView(view);
}

void View::SetRootNode(IntrusivePtr<Node> node)
{ 
    rootNode = node;
}

void View::StartFrame()
{ 
    renderer->StartFrame();
}
