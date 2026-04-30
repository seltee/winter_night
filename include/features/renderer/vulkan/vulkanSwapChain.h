#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanQueueFamilies.h"
#include "features/renderer/vulkan/vulkanDeviceExtensions.h"
#include <vector>

namespace WNE
{
    class VulkanSwapChain
    {
    public:
        VulkanSwapChain(VkDevice device);
        ~VulkanSwapChain();
        bool setup(int width, int height, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, bool isImmidiateSwap);

        inline unsigned int getImageFormat()
        {
            return swapChainImageFormat;
        }

        inline VkExtent2D *getExtent()
        {
            return swapChainExtent;
        }

        inline std::vector<VkImageView> *getImageViews()
        {
            return &swapChainImageViews;
        }

        inline VkSwapchainKHR getSwapChain()
        {
            return swapChain;
        }

        static bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);

    protected:
        std::vector<VkImageView> swapChainImageViews;
        VkSwapchainKHR swapChain = nullptr;
        unsigned int swapChainImageFormat;
        VkExtent2D *swapChainExtent;
        VkDevice device;

        VkSwapchainKHR createSwapChain(
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            VkSurfaceKHR surface,
            int nWindowWidth,
            int nWindowHeight,
            unsigned int *swapChainImageFormat,
            VkExtent2D *swapChainExtent,
            unsigned int *punImageCount, 
            bool isImmidiateSwap);

        bool createSwapChainImages(
            VkDevice device,
            VkSwapchainKHR swapChain,
            unsigned int unImageCount,
            int swapChainImageFormat,
            std::vector<VkImage> *swapChainImages,
            std::vector<VkImageView> *swapChainImageViews);
    };
}