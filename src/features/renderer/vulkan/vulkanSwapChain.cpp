#include "features/renderer/vulkan/vulkanSwapChain.h"
#include "features/renderer/vulkan/vulkanQueueFamilies.h"
#include "features/renderer/vulkan/vulkanDeviceExtensions.h"
#include "features/renderer/vulkan/vulkanUtils.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/logger/logger.h"
#include "vulkan/vulkan.h"

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

VulkanSwapChain::VulkanSwapChain(VulkanUtils *vulkanUtils)
{
    this->vulkanDevice = vulkanUtils->getVulkanDevice();
    this->vulkanUtils = vulkanUtils;
}

VulkanSwapChain::~VulkanSwapChain()
{
    auto device = vulkanDevice->getDevice();
    for (auto &image : swapChainSampledImages)
        vkDestroyImage(device, image, nullptr);

    if (swapChain)
        vkDestroySwapchainKHR(device, swapChain, nullptr);
}

bool VulkanSwapChain::setup(int width, int height, VkSurfaceKHR surface, bool isImmidiateSwap, uint MSAASampleCount)
{
    auto device = vulkanDevice->getDevice();

    unsigned int unImageCount;

    swapChain = createSwapChain(surface, width, height, &swapChainImageFormat, &unImageCount, isImmidiateSwap);
    if (!swapChain)
    {
        Logger::log << "Unable to create swap chain" << endl;
        return false;
    }

    std::vector<VkImage> swapChainImages;
    vkGetSwapchainImagesKHR(device, swapChain, &unImageCount, nullptr);
    swapChainImages.resize(unImageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &unImageCount, swapChainImages.data());

    if (!createSwapChainImages(swapChain, unImageCount, (VkFormat)swapChainImageFormat, &swapChainImages))
    {
        Logger::log << "Unable to create swap chain images" << endl;
        return false;
    }

    if (MSAASampleCount > 1)
    {
        swapChainSampledImages.resize(unImageCount);
        swapChainSampledMemory.resize(unImageCount);
        swapChainSampledImageViews.resize(unImageCount);

        for (uint i = 0; i < unImageCount; i++)
        {
            vulkanUtils->createImage(
                width, height,
                (VkFormat)swapChainImageFormat,
                vulkanUtils->getVkSampleCountFlagBits(MSAASampleCount),
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                &swapChainSampledImages[i],
                &swapChainSampledMemory[i]);

            auto vulkanImagePtr = std::make_unique<VulkanImageView>(vulkanDevice);
            if (!vulkanImagePtr->setup(swapChainSampledImages[i], (VkFormat)swapChainImageFormat))
            {
                Logger::log << "Failed to create MSAA color image view" << endl;
                return false;
            }
            swapChainSampledImageViews[i] = std::move(vulkanImagePtr);
        }
    }

    return true;
}

VkSwapchainKHR VulkanSwapChain::createSwapChain(
    VkSurfaceKHR vulkanSurface,
    int windowWidth,
    int windowHeight,
    uint *swapChainImageFormat,
    uint *punImageCount,
    bool isImmidiateSwap)
{
    VkResult result;
    auto physicalDevice = vulkanDevice->getPhysicalDevice();
    auto device = vulkanDevice->getDevice();

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, vulkanSurface, &capabilities);

    SwapChainSupportDetails swapChainSupport = _querySwapChainSupport(physicalDevice, vulkanSurface);
    VkSurfaceFormatKHR surfaceFormat = _chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = _chooseSwapPresentMode(swapChainSupport.presentModes, isImmidiateSwap);

    int imageCount = capabilities.minImageCount + 1 < capabilities.maxImageCount
                         ? capabilities.minImageCount + 1
                         : capabilities.minImageCount;

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = vulkanSurface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent.width = windowWidth;
    createInfo.imageExtent.height = windowHeight;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = 1;

    /*
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
    */

    VkSwapchainKHR swapChain;
    result = vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain);
    if (result != VK_SUCCESS)
    {
        Logger::log << "Unable to create swapchain" << endl;
        return nullptr;
    }

    *swapChainImageFormat = surfaceFormat.format;
    swapChainExtent.width = windowWidth;
    swapChainExtent.height = windowHeight;

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
            Logger::log << "Unable to create swap chain image" << endl;
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

    uint32_t formatCount = 0;
    VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    if (result != VK_SUCCESS || formatCount == 0)
        Logger::log << "Failed to get physical device surface formats, count: " << formatCount << endl;

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
#if defined(OS_LINUX)
    return VK_PRESENT_MODE_MAILBOX_KHR;
#else
    if (isImmidiateSwap)
    {
        return VK_PRESENT_MODE_IMMEDIATE_KHR;
    }
    else
    {
        return VK_PRESENT_MODE_FIFO_KHR;
    }
#endif

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
