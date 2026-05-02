#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"

namespace wne
{
    class VulkanCommandPool
    {
    public:
        VulkanCommandPool(VkDevice device, VkPhysicalDevice physicalDevice);
        ~VulkanCommandPool();

        bool setup(VkSurfaceKHR surface);

        inline VkCommandPool getCommandPool()
        {
            return commandPool;
        }

    protected:
        VkCommandPool commandPool;
        VkPhysicalDevice physicalDevice;
        VkDevice device;
    };
};
