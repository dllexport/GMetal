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
    boost::intrusive_ptr<VulkanContext> DefaultBuild();
    VulkanContextBuilder& SetInstanceExtensions(std::vector<const char*>&& instanceExtensions);
    VulkanContextBuilder& SetInstanceLayers(std::vector<const char*>&& instanceLayers);
    VulkanContextBuilder& SetDeviceExtensions(std::vector<const char*>&& extensions);
    VulkanContextBuilder& SetDeviceLayers(std::vector<const char*>&& layers);
    VulkanContextBuilder& EnableValidationLayer();
    VulkanContextBuilder& BuildDebugUtilsMessenger(PFN_vkDebugUtilsMessengerCallbackEXT callback);
    VulkanContextBuilder& SelectPhysicalDevice(std::function<int(std::vector<VkPhysicalDevice>)> selector);

private:
    boost::intrusive_ptr<VulkanContext> context;
    std::vector<const char*> instanceExtensions;
    std::vector<const char*> instanceLayers;
    std::vector<const char*> deviceExtensions;
    std::vector<const char*> deviceLayers;

    std::vector<VkExtensionProperties> availableInstanceExtensions;
    std::vector<VkLayerProperties> availableInstanceLayers;
    bool enableValidationLayers;
    PFN_vkDebugUtilsMessengerCallbackEXT debugUtilsMessengerCallback;

    // pick discrete GPU by default
    static int DefaultPhysicalDeviceSelector(std::vector<VkPhysicalDevice> devices);
    std::function<int(std::vector<VkPhysicalDevice>)> physicalDeviceSelector = DefaultPhysicalDeviceSelector;

    void BuildInstance();
    void BuildPhysicalDevice();
    void BuildLogicalDevice();
};


