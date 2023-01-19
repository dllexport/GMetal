//
// Created by Mario on 2023/1/18.
//
#pragma once

#include <vector>

#include <VulkanContext.h>

class VulkanContextBuilder {
public:
    VulkanContextBuilder();

    boost::intrusive_ptr<VulkanContext> Build();
    VulkanContextBuilder& SetInstanceExtensions(std::vector<const char*>&& instanceExtensions);
    VulkanContextBuilder& SetInstanceLayers(std::vector<const char*>&& instanceLayers);
    VulkanContextBuilder& BuildInstance();

private:
    VulkanContext* context;
    std::vector<const char*> instanceExtensions;
    std::vector<const char*> instanceLayers;

    std::vector<VkExtensionProperties> availableInstanceExtensions;
    std::vector<const char*> availableInstanceLayers;
};


