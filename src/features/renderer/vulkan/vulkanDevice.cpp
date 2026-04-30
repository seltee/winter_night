#include "features/renderer/vulkan/vulkanDevice.h"
#include "features/renderer/vulkan/vulkanSwapChain.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <iostream>

using namespace WNE;

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
        std::cout << "No suitable Vulkan devices found" << std::endl;
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
        std::cout << "No suitable Vulkan device found" << std::endl;
        return false;
    }

    std::sort(pickedDevices.begin(), pickedDevices.end(), [](const VulkanDeviceInfo &a, const VulkanDeviceInfo &b)
              { return a.score > b.score; });
    physicalDevice = pickedDevices.at(0).device;

    VulkanQueueFamilies vulkanQueueFamilies;
    if (!vulkanQueueFamilies.setup(physicalDevice, surface))
    {
        std::cout << "Unable to setup queue families" << std::endl;
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

    VkPhysicalDeviceFeatures deviceFeatures{};
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
        std::cout << "Unable to create physical device " << result << std::endl;
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
