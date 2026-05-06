#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#include "core/core.h"
#include <memory>
namespace wne
{
    class VulkanCommandPool;
    class VulkanDescriptorPool;
    class VulkanDescriptorSet;
    class VulkanSampler;
    class VulkanPipeline;

    class VulkanUtils
    {
    public:
        VulkanUtils(VulkanDevice *vulkanDevice, VulkanCommandPool *vulkanCommandPool, VulkanPipeline *vulkanPipeline, VkQueue graphicsQueue, VkQueue presentQueue);
        ~VulkanUtils();
        bool setup();

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

        inline VulkanPipeline *getVulkanPipeline()
        {
            return vulkanPipeline;
        }

        inline void setVulkanPipeline(VulkanPipeline *newVulkanPipeline)
        {
            vulkanPipeline = newVulkanPipeline;
        }

        inline bool isAnisotropySupported()
        {
            return anisotropySupported;
        }

        inline VulkanSampler *getSampler()
        {
            return vulkanSampler.get();
        }

        inline VkCommandBuffer getCurrentCommandBuffer()
        {
            return currentCommandBuffer;
        }

        inline void setCurrentCommandBuffer(VkCommandBuffer newCommandBuffer)
        {
            currentCommandBuffer = newCommandBuffer;
        }

    protected:
        std::unique_ptr<VulkanDescriptorPool> vulkanDescriptorPool;
        std::unique_ptr<VulkanSampler> vulkanSampler;

        VulkanDevice *vulkanDevice = nullptr;
        VulkanCommandPool *vulkanCommandPool = nullptr;
        VulkanPipeline *vulkanPipeline = nullptr;
        VkCommandBuffer currentCommandBuffer = nullptr;

        VkDevice device;
        VkPhysicalDevice physicalDevice;
        VkQueue graphicsQueue = nullptr;
        VkQueue presentQueue = nullptr;

        bool anisotropySupported = false;

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    };
};