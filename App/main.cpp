#ifdef _WIN32
//
// Created by Mario on 2023/1/18.
//
#include <iostream>
#include <VulkanContext.h>
#include <spdlog/spdlog.h>
#include <View.h>

#include <VulkanContextBuilder.h>
#include <SurfaceBuilder.h>
#include <SwapChainBuilder.h>
#include <PipelineBuilder.h>
#include <RenderPassBuilder.h>
#include <DescriptorPoolBuilder.h>

#include <Renderer.h>
#include <Drawable.h>
#include <Geometry.h>

#include <MeshExchange.h>

#include <Vertex.h>
#include <Windows.h>

int main()
{
    spdlog::set_level(spdlog::level::info);

    auto geometry = MeshExchange::LoadObj(
            "C:/Users/Mario/Documents/GitHub/LearnOpenGL/resources/objects/backpack/backpack.obj");
    
    auto view = gmetal::make_intrusive<View>();
    view->SetRootNode(geometry);

    //auto drawable = gmetal::make_intrusive<Drawable>();
    //drawable->SetVertexArray({});
    //drawable->SetVertexTopology(Drawable::DrawTopology::TRIANGLE_LIST, Drawable::UINT16);

    while (1)
    {
        MSG msg;
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        view->StartFrame();
    }
  
    return 0;
}

#endif