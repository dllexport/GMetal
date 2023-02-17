//
// Created by Mario on 2023/1/18.
//

#include <VulkanContextBuilder.h>

#include <unordered_set>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan.hpp>

#include <ValidationLayerDebugCallback.h>

VulkanContextBuilder::VulkanContextBuilder()
{
    this->context = new VulkanContext{};
    enableValidationLayers = false;
    debugUtilsMessengerCallback = ValidationLayerDebugCallback;

    {
        uint32_t count = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
        availableInstanceExtensions.resize(count);
        vkEnumerateInstanceExtensionProperties(nullptr, &count, availableInstanceExtensions.data());

#ifndef NDEBUG
        spdlog::debug("available extensions:");
        for (auto& extension : availableInstanceExtensions)
        {
            spdlog::debug("{}:{}", extension.extensionName, extension.specVersion);
        }
#endif
    }

    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        availableInstanceLayers.resize(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableInstanceLayers.data());
    }
}

boost::intrusive_ptr<VulkanContext> VulkanContextBuilder::Build()
{
    BuildInstance();
    BuildPhysicalDevice();
    BuildLogicalDevice();
    BuildVMA();
    return this->context;
}

boost::intrusive_ptr<VulkanContext> VulkanContextBuilder::DefaultBuild()
{
    return SetInstanceExtensions({}).EnableValidationLayer().SetInstanceLayers({"VK_LAYER_KHRONOS_validation"}).SetDeviceExtensions({VK_KHR_SWAPCHAIN_EXTENSION_NAME}).Build();
}

VulkanContextBuilder &VulkanContextBuilder::SetInstanceExtensions(std::vector<const char *> &&instanceExtensions)
{
    this->instanceExtensions = instanceExtensions;
    return *this;
}

VulkanContextBuilder &VulkanContextBuilder::SetInstanceLayers(std::vector<const char *> &&instanceLayers)
{
    this->instanceLayers = instanceLayers;
    return *this;
}

VulkanContextBuilder &VulkanContextBuilder::SetDeviceExtensions(std::vector<const char *> &&extensions)
{
    this->deviceExtensions = extensions;
    return *this;
}

VulkanContextBuilder &VulkanContextBuilder::SetDeviceLayers(std::vector<const char *> &&layers)
{
    this->deviceLayers = layers;
    return *this;
}

VulkanContextBuilder &VulkanContextBuilder::EnableValidationLayer()
{
    enableValidationLayers = true;
    instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    return *this;
}

void VulkanContextBuilder::BuildInstance()
{
    // check layer
    for (auto requiredLayer : instanceLayers)
    {
        auto result = std::find_if(availableInstanceLayers.begin(), availableInstanceLayers.end(), [&](VkLayerProperties &value)
                                   { return std::string(requiredLayer) == std::string(value.layerName); });

        if (result == availableInstanceLayers.end())
        {
            spdlog::info("layer {} not support", requiredLayer);
        }
    }

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

    ici.enabledLayerCount = instanceLayers.size();
    ici.ppEnabledLayerNames = instanceLayers.data();

    auto result = vkCreateInstance(&ici, nullptr, &context->instance);
    if (result != VK_SUCCESS)
    {
        spdlog::error("vkCreateInstance failed.");
        exit(-1);
    }

    if (!enableValidationLayers)
    {
        return;
    }

    VkDebugUtilsMessengerCreateInfoEXT info = {};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    info.pfnUserCallback = debugUtilsMessengerCallback;
    info.pUserData = nullptr;

    if (!context->instance)
    {
        throw std::runtime_error("Init instance before BuildDebugUtilsMessenger");
    }

    auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context->instance, "vkCreateDebugUtilsMessengerEXT");
    if (!vkCreateDebugUtilsMessengerEXT)
    {
        throw std::runtime_error("Failed to resolve vkCreateDebugUtilsMessengerEXT");
    }

    if (vkCreateDebugUtilsMessengerEXT(context->instance, &info, nullptr, &context->debugMessenger) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to set up debug messenger");
    }
}

VulkanContextBuilder &VulkanContextBuilder::BuildDebugUtilsMessenger(PFN_vkDebugUtilsMessengerCallbackEXT callback)
{
    this->debugUtilsMessengerCallback = callback;
    if (!callback)
    {
        debugUtilsMessengerCallback = ValidationLayerDebugCallback;
    }
    return *this;
}

VulkanContextBuilder &VulkanContextBuilder::SelectPhysicalDevice(std::function<int(std::vector<VkPhysicalDevice>)> selector)
{
    physicalDeviceSelector = selector;
    return *this;
}

int VulkanContextBuilder::DefaultPhysicalDeviceSelector(std::vector<VkPhysicalDevice> devices)
{
    for (int i = 0; i < devices.size(); i++)
    {
        auto device = devices[i];
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            return i;
        }
    }
    return -1;
}

void VulkanContextBuilder::BuildPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(context->instance, &deviceCount, nullptr);
    if (!deviceCount)
    {
        throw std::runtime_error("Failed to find GPUs with Vulkan support");
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(context->instance, &deviceCount, devices.data());
    auto selectIndex = physicalDeviceSelector(devices);
    if (selectIndex == -1)
    {
        throw std::runtime_error("Failed to find a suitable GPU");
    }
    context->physicalDevice = devices[selectIndex];

    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(context->physicalDevice, nullptr, &extensionCount, nullptr);
    availablePhysicalDeviceExtensions.resize(extensionCount);
    vkEnumerateDeviceExtensionProperties(context->physicalDevice, nullptr, &extensionCount, availablePhysicalDeviceExtensions.data());

#ifndef NDEBUG
    spdlog::debug("available device extensions:");
    for (auto& extension : availablePhysicalDeviceExtensions)
    {
        spdlog::debug("{}:{}", extension.extensionName, extension.specVersion);
    }
#endif

}

std::unordered_map<VkFlags, uint32_t> SelectDeviceQueues(std::vector<VkQueueFamilyProperties> &properties)
{
    std::unordered_map<VkFlags, uint32_t> result;
    for (int i = 0; i < properties.size(); i++)
    {
        auto &p = properties[i];
        spdlog::info("queue {}: {}", i, vk::to_string(vk::QueueFlags(p.queueFlags)));

        if ((p.queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT) && (p.queueFlags & VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT) && (p.queueFlags & VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT))
        {
            if (!result.count(VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT))
                result[VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT] = i;
        }

        // try find unique compute queue
        if (!(p.queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT) && (p.queueFlags & VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT))
        {
            if (!result.count(VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT))
                result[VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT] = i;
        }

        // try find unique transfer queue
        if (!(p.queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT) && !(p.queueFlags & VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT) && (p.queueFlags & VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT))
        {
            if (!result.count(VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT))
                result[VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT] = i;
        }
    }

    if (!result.count(VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT))
    {
        throw std::runtime_error("Cannot find VK_QUEUE_GRAPHICS_BIT");
    }

    if (!result.count(VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT))
    {
        result[VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT] = result[VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT];
    }

    if (!result.count(VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT))
    {
        result[VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT] = result[VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT];
    }

    return result;
}

static std::unordered_set<uint32_t> ToUniqueQueueFamilySet(std::unordered_map<VkFlags, uint32_t> &queueMap)
{
    std::unordered_set<uint32_t> result;
    for (auto &[k, v] : queueMap)
    {
        result.insert(v);
    }
    return result;
}

void VulkanContextBuilder::BuildLogicalDevice()
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(context->physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(context->physicalDevice, &queueFamilyCount, queueFamilies.data());

    auto selectedQueues = SelectDeviceQueues(queueFamilies);

    std::vector<VkDeviceQueueCreateInfo> dqcis;

    float queuePriority = 1.0f;
    for (auto &v : ToUniqueQueueFamilySet(selectedQueues))
    {
        VkDeviceQueueCreateInfo dqci = {};
        dqci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        dqci.queueFamilyIndex = v;
        dqci.queueCount = 1;
        dqci.pQueuePriorities = &queuePriority;
        dqcis.push_back(dqci);
    }

    VkPhysicalDeviceFeatures pdf = {};

    VkDeviceCreateInfo dci = {};
    dci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    dci.queueCreateInfoCount = dqcis.size();
    dci.pQueueCreateInfos = dqcis.data();
    dci.pEnabledFeatures = &pdf;
    dci.enabledExtensionCount = deviceExtensions.size();
    dci.ppEnabledExtensionNames = deviceExtensions.data();
    if (enableValidationLayers)
    {
        dci.enabledLayerCount = deviceLayers.size();
        dci.ppEnabledLayerNames = deviceLayers.data();
    }

    if (vkCreateDevice(context->physicalDevice, &dci, nullptr, &context->logicalDevice) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create logical device!");
    }

    VkQueue graphicsQueue;
    VkQueue computeQueue;
    VkQueue transferQueue;

    vkGetDeviceQueue(context->logicalDevice, selectedQueues[VK_QUEUE_GRAPHICS_BIT], 0, &graphicsQueue);
    vkGetDeviceQueue(context->logicalDevice, selectedQueues[VK_QUEUE_COMPUTE_BIT], 0, &computeQueue);
    vkGetDeviceQueue(context->logicalDevice, selectedQueues[VK_QUEUE_TRANSFER_BIT], 0, &transferQueue);

    context->queueContextMap[VK_QUEUE_GRAPHICS_BIT] = {graphicsQueue, selectedQueues[VK_QUEUE_GRAPHICS_BIT]};
    context->queueContextMap[VK_QUEUE_COMPUTE_BIT] = {computeQueue, selectedQueues[VK_QUEUE_COMPUTE_BIT]};
    context->queueContextMap[VK_QUEUE_TRANSFER_BIT] = {transferQueue, selectedQueues[VK_QUEUE_TRANSFER_BIT]};
}

void VulkanContextBuilder::BuildVMA()
{
    VmaAllocatorCreateInfo allocatorCreateInfo = {};
    allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_2;
    allocatorCreateInfo.physicalDevice = context->physicalDevice;
    allocatorCreateInfo.device = context->logicalDevice;
    allocatorCreateInfo.instance = context->instance;

    vmaCreateAllocator(&allocatorCreateInfo, &context->vmaAllocator);
}
