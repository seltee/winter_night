#include "features/renderer/vulkan/vulkanSwapChain.h"
#include "features/renderer/vulkan/vulkanQueueFamilies.h"
#include "features/renderer/vulkan/vulkanDeviceExtensions.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <iostream>

using namespace wne;

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

SwapChainSupportDetails _querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
VkSurfaceFormatKHR _chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
VkPresentModeKHR _chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes, bool isImmidiateSwap);
VkExtent2D _chooseSwapExtent(int screenWidth, int screenHeight);
bool _checkDeviceExtensionSupport(VkPhysicalDevice device);

VulkanSwapChain::VulkanSwapChain(VulkanDevice *vulkanDevice)
{
    this->vulkanDevice = vulkanDevice;
}

VulkanSwapChain::~VulkanSwapChain()
{
    if (swapChain)
        vkDestroySwapchainKHR(vulkanDevice->getDevice(), swapChain, nullptr);
}

bool VulkanSwapChain::setup(int width, int height, VkSurfaceKHR surface, bool isImmidiateSwap)
{
    auto device = vulkanDevice->getDevice();

    unsigned int unImageCount;

    swapChain = createSwapChain(surface, width, height, &swapChainImageFormat, &unImageCount, isImmidiateSwap);
    if (!swapChain)
    {
        std::cout << "Unable to create swap chain" << std::endl;
        return false;
    }

    std::vector<VkImage> swapChainImages;
    vkGetSwapchainImagesKHR(device, swapChain, &unImageCount, nullptr);
    swapChainImages.resize(unImageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &unImageCount, swapChainImages.data());

    if (!createSwapChainImages(swapChain, unImageCount, (VkFormat)swapChainImageFormat, &swapChainImages))
    {
        std::cout << "Unable to create swap chain images" << std::endl;
        return false;
    }

    return true;
}

VkSwapchainKHR VulkanSwapChain::createSwapChain(
    VkSurfaceKHR surface,
    int nWindowWidth,
    int nWindowHeight,
    unsigned int *swapChainImageFormat,
    unsigned int *punImageCount,
    bool isImmidiateSwap)
{
    auto physicalDevice = vulkanDevice->getPhysicalDevice();
    auto device = vulkanDevice->getDevice();
    SwapChainSupportDetails swapChainSupport = _querySwapChainSupport(physicalDevice, surface);

    VkSurfaceFormatKHR surfaceFormat = _chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = _chooseSwapPresentMode(swapChainSupport.presentModes, isImmidiateSwap);

    VkExtent2D extent = _chooseSwapExtent(nWindowWidth, nWindowHeight);

    uint32_t unImageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && unImageCount > swapChainSupport.capabilities.maxImageCount)
        unImageCount = swapChainSupport.capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = unImageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    VulkanQueueFamilies vulkanQueueFamilies;
    vulkanQueueFamilies.setup(physicalDevice, surface);

    uint32_t queueFamilyIndices[] = {vulkanQueueFamilies.getGraphicsFamily().value(), vulkanQueueFamilies.getPresentFamily().value()};

    if (vulkanQueueFamilies.getGraphicsFamily() != vulkanQueueFamilies.getPresentFamily())
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;     // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkSwapchainKHR swapChain;
    VkResult result = vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain);
    if (result != VK_SUCCESS)
        return nullptr;
    *swapChainImageFormat = surfaceFormat.format;
    memcpy(&swapChainExtent, &extent, sizeof(extent));
    *punImageCount = unImageCount;
    return swapChain;
}

bool VulkanSwapChain::createSwapChainImages(
    VkSwapchainKHR swapChain,
    unsigned int unImageCount,
    int swapChainImageFormat,
    std::vector<VkImage> *swapChainImages)
{
    auto device = vulkanDevice->getDevice();

    vkGetSwapchainImagesKHR(device, swapChain, &unImageCount, nullptr);
    swapChainImages->resize(unImageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &unImageCount, swapChainImages->data());

    for (size_t i = 0; i < unImageCount; i++)
    {
        auto vulkanImagePtr = std::make_unique<VulkanImageView>(vulkanDevice);
        if (!vulkanImagePtr->setup(swapChainImages->at(i), swapChainImageFormat))
        {
            std::cout << "Unable to create swap chain image" << std::endl;
            return false;
        }
        swapChainImageViews.emplace_back(std::move(vulkanImagePtr));
    }

    return true;
}

bool VulkanSwapChain::isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    VulkanQueueFamilies family;
    family.setup(device, surface);

    bool extensionsSupported = _checkDeviceExtensionSupport(device);
    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        SwapChainSupportDetails swapChainSupport = _querySwapChainSupport(device, surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return deviceFeatures.geometryShader && family.isComplete() && extensionsSupported && swapChainAdequate;
}

SwapChainSupportDetails _querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR _chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
    for (const auto &availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

VkPresentModeKHR _chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes, bool isImmidiateSwap)
{
    if (isImmidiateSwap)
    {
        return VK_PRESENT_MODE_IMMEDIATE_KHR;
    }
    else
    {
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    // todo
    // VK_PRESENT_MODE_MAILBOX_KHR for desctop devices, VK_PRESENT_MODE_FIFO_KHR is for mobile
}

VkExtent2D _chooseSwapExtent(int nScreenWidth, int nScreenHeight)
{
    VkExtent2D actualExtent = {
        static_cast<uint32_t>(nScreenWidth),
        static_cast<uint32_t>(nScreenHeight)};
    return actualExtent;
}

bool _checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    VulkanDeviceExtensions *deviceExtensions = new VulkanDeviceExtensions(device);
    bool hasExtSupport = deviceExtensions->hasExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    delete deviceExtensions;
    return hasExtSupport;
}
