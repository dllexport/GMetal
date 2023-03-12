//
// Created by Mario on 2023/1/19.
//

#pragma once

#include <vulkan/vulkan.h>
#include <spdlog/spdlog.h>

VKAPI_ATTR VkBool32 VKAPI_CALL ValidationLayerDebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

    spdlog::level::level_enum level = spdlog::level::debug;
    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: {
            level = spdlog::level::trace;
            break;
        }
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
            level = spdlog::level::info;
            break;
        }
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
            level = spdlog::level::warn;
            break;
        }
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
            level = spdlog::level::err;
            break;
        }
        default:{
            break;
        }
    }

    spdlog::log(level, "{}", pCallbackData->pMessage);
    return VK_FALSE;
}
