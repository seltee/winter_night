#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanQueueFamilies.h"
#include "features/renderer/vulkan/vulkanRenderPass.h"
#include "features/renderer/vulkan/vulkanFrameBuffer.h"
#include "features/renderer/vulkan/vulkanPipeline.h"

namespace WNE
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
