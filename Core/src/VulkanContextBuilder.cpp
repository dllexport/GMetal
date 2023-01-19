//
// Created by Mario on 2023/1/18.
//

#include <VulkanContextBuilder.h>
#include <spdlog/spdlog.h>

VulkanContextBuilder::VulkanContextBuilder()
{
    this->context = new VulkanContext{};
}

boost::intrusive_ptr<VulkanContext> VulkanContextBuilder::Build()
{
    return this->context;
}

VulkanContextBuilder& VulkanContextBuilder::SetInstanceExtensions(std::vector<const char*>&& instanceExtensions)
{
    this->instanceExtensions = instanceExtensions;

    uint32_t count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
    availableInstanceExtensions.resize(count);
    vkEnumerateInstanceExtensionProperties(nullptr, &count, availableInstanceExtensions.data());

    spdlog::debug("available extensions:");
    for (auto& extension : availableInstanceExtensions) {
        spdlog::debug("{}:{}", extension.extensionName, extension.specVersion);
    }

    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::SetInstanceLayers(std::vector<const char*>&& instanceLayers)
{
    this->instanceLayers = instanceLayers;
    return *this;
}

VulkanContextBuilder& VulkanContextBuilder::BuildInstance()
{
    VkApplicationInfo ai = {};
    ai.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    ai.pApplicationName = "GMetalCore";
    ai.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    ai.pEngineName = "GMetalCore";
    ai.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    ai.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo ici = {};
    ici.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    ici.pApplicationInfo = &ai;

    ici.enabledExtensionCount = instanceExtensions.size();
    ici.ppEnabledExtensionNames = instanceExtensions.data();

    auto result = vkCreateInstance(&ici, nullptr, &context->instance);
    if (result != VK_SUCCESS)
    {
        spdlog::error("vkCreateInstance failed.");
        exit(-1);
    }
    return *this;
}
