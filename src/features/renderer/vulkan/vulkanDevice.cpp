#include "features/renderer/vulkan/vulkanDevice.h"
#include "features/renderer/vulkan/vulkanSwapChain.h"
#include "features/renderer/vulkan/vulkanShader.h"
#include "features/renderer/vulkan/vulkanRenderPass.h"
#include "features/renderer/vulkan/vulkanQueueFamilies.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/logger/logger.h"
#include "vulkan/vulkan.h"

using namespace wne;

typedef struct VulkanDeviceInfo
{
    VkPhysicalDevice device;
    int score;
} VulkanDeviceInfo;

int _getDeviceScore(VkPhysicalDevice device);

VulkanDevice::VulkanDevice(VkInstance instance, VkSurfaceKHR surface)
{
    this->instance = instance;
    this->surface = surface;
}

VulkanDevice::~VulkanDevice()
{
    if (device)
        vkDestroyDevice(device, nullptr);
}

bool VulkanDevice::setup()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        Logger::log << "No suitable Vulkan devices found" << endl;
        return false;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    std::vector<VulkanDeviceInfo> pickedDevices;
    for (const auto &device : devices)
    {
        if (VulkanSwapChain::isDeviceSuitable(device, surface))
        {
            pickedDevices.push_back(VulkanDeviceInfo({device, _getDeviceScore(device)}));
        }
    }
    if (pickedDevices.size() == 0)
    {
        Logger::log << "No suitable Vulkan device found" << endl;
        return false;
    }

    std::sort(pickedDevices.begin(), pickedDevices.end(), [](const VulkanDeviceInfo &a, const VulkanDeviceInfo &b)
              { return a.score > b.score; });
    physicalDevice = pickedDevices.at(0).device;

    Logger::log << "Video device list:" << endl;
    for (const auto &device : pickedDevices)
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device.device, &deviceProperties);
        Logger::log << deviceProperties.deviceName << " API: " << deviceProperties.apiVersion << " Score: " << device.score << endl;
    }

    VulkanQueueFamilies vulkanQueueFamilies;
    if (!vulkanQueueFamilies.setup(physicalDevice, surface))
    {
        Logger::log << "Unable to setup queue families" << endl;
        return false;
    }

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {vulkanQueueFamilies.getGraphicsFamily().value(), vulkanQueueFamilies.getPresentFamily().value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);

    VkPhysicalDeviceFeatures deviceFeatures{};
    if (supportedFeatures.samplerAnisotropy)
        deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    deviceCreateInfo.enabledExtensionCount = VULKAN_DEVICE_REQUIRED_EXTENSIONS;
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtNames;

    deviceCreateInfo.enabledLayerCount = 0;

    VkResult result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
    if (result != VK_SUCCESS)
    {
        Logger::log << "Unable to create physical device " << result << endl;
        return false;
    }

    return true;
}

int _getDeviceScore(VkPhysicalDevice device)
{
    int score = 0;

    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        score += 2000;
    }
    score += deviceProperties.limits.maxImageDimension2D;

    return score;
}
