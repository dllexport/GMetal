//
// Created by Mario on 2023/1/18.
//
#include <iostream>
#include <VulkanContext.h>
#include <VulkanContextBuilder.h>
#include <spdlog/spdlog.h>

int main()
{
    spdlog::set_level(spdlog::level::info);

    VulkanContextBuilder vulkanContextBuilder;
    auto vkContext =
            vulkanContextBuilder
            .SetInstanceExtensions({})
            .EnableValidationLayer()
            .SetInstanceLayers({"VK_LAYER_KHRONOS_validation"})
            .Build();
    return 0;
}