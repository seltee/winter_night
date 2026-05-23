#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanImageView.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#include <memory>
#include <vector>

namespace wne
{
    class VulkanUtils;
    typedef struct Extent
    {
        uint32 width;
        uint32 height;
    } Extent;

    class VulkanSwapChain
    {
    public:
        VulkanSwapChain(VulkanUtils *vulkanUtils);
        ~VulkanSwapChain();
        bool setup(int width, int height, VkSurfaceKHR surface, bool isImmidiateSwap, uint MSAASampleCount);

        inline unsigned int getImageFormat()
        {
            return swapChainImageFormat;
        }

        inline VkExtent2D *getExtent()
        {
            return (VkExtent2D *)&swapChainExtent;
        }

        inline std::vector<std::unique_ptr<VulkanImageView>> *getImageViews()
        {
            return &swapChainImageViews;
        }

        inline std::vector<std::unique_ptr<wne::VulkanImageView>> *getSampledImageViews()
        {
            return &swapChainSampledImageViews;
        }

        inline VkSwapchainKHR getSwapChain()
        {
            return swapChain;
        }

        static bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);

    protected:
        VulkanDevice *vulkanDevice = nullptr;
        VulkanUtils *vulkanUtils = nullptr;

        std::vector<std::unique_ptr<VulkanImageView>> swapChainImageViews;

        std::vector<VkImage> swapChainSampledImages;
        std::vector<VkDeviceMemory> swapChainSampledMemory;
        std::vector<std::unique_ptr<VulkanImageView>> swapChainSampledImageViews;

        VkSwapchainKHR swapChain = nullptr;
        unsigned int swapChainImageFormat;
        Extent swapChainExtent{};

        VkSwapchainKHR createSwapChain(
            VkSurfaceKHR surface,
            int nWindowWidth,
            int nWindowHeight,
            unsigned int *swapChainImageFormat,
            unsigned int *punImageCount,
            bool isImmidiateSwap);

        bool createSwapChainImages(
            VkSwapchainKHR swapChain,
            unsigned int unImageCount,
            int swapChainImageFormat,
            std::vector<VkImage> *swapChainImages);
    };
}