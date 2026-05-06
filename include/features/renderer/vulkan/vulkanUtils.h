#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "core/core.h"
namespace wne
{
    class VulkanDevice;
    class VulkanCommandPool;

    class VulkanUtils
    {
    public:
        VulkanUtils(VulkanDevice *vulkanDevice, VulkanCommandPool *vulkanCommandPool, VkQueue graphicsQueue, VkQueue presentQueue);
        ~VulkanUtils();

        int64 findMemoryType(uint32 typeFilter, uint64 properties) noexcept;
        bool createBuffer(uint64 size, uint32 usage, uint32 properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
        bool copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, uint64 size);
        void transitionImageLayout(VkImage image, uint64 format, uint64 oldLayout, uint64 newLayout);
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32 width, uint32 height);

        inline VulkanDevice *getVulkanDevice()
        {
            return vulkanDevice;
        }

        inline VulkanCommandPool *getVulkanCommandPool()
        {
            return vulkanCommandPool;
        }

        inline bool isAnisotropySupported()
        {
            return anisotropySupported;
        }

    protected:
        VkDevice device;
        VkPhysicalDevice physicalDevice;
        VulkanDevice *vulkanDevice = nullptr;
        VulkanCommandPool *vulkanCommandPool = nullptr;
        VkQueue graphicsQueue = nullptr;
        VkQueue presentQueue = nullptr;

        bool anisotropySupported = false;

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    };
};