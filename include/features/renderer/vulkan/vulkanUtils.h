#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#include "features/renderer/vulkan/vulkanCommandPool.h"

namespace WNE
{
    class VulkanUtils
    {
    public:
        VulkanUtils(VulkanDevice *vulkanDevice, VulkanCommandPool *vulkanCommandPool, VkQueue graphicsQueue, VkQueue presentQueue);
        ~VulkanUtils();

        int64 findMemoryType(uint32 typeFilter, uint64 properties) noexcept;
        bool createBuffer(uint64 size, uint32 usage, uint32 properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
        bool copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, uint64 size);

        inline VulkanDevice *getVulkanDevice()
        {
            return vulkanDevice;
        }

        inline VulkanCommandPool *getVulkanCommandPool()
        {
            return vulkanCommandPool;
        }

    protected:
        VkDevice device;
        VkPhysicalDevice physicalDevice;
        VulkanDevice *vulkanDevice = nullptr;
        VulkanCommandPool *vulkanCommandPool = nullptr;
        VkQueue graphicsQueue = nullptr;
        VkQueue presentQueue = nullptr;
    };
};