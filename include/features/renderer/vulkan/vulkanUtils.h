#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#include "features/renderer/vulkan/vulkanCommandBuffer.h"
#include "features/renderer/vulkan/pipelines/vulkanPipeline.h"
#include "features/renderer/vulkan/pipelines/vulkanPipelineColored.h"
#include "features/renderer/vulkan/pipelines/vulkanPipelineTextured.h"
#include "features/renderer/vulkan/vulkanShader.h"
#include "features/data/model.h"

#include "core/core.h"
#include <memory>
#include <iostream>

namespace wne
{
    class VulkanCommandPool;
    class VulkanDescriptorPool;
    class VulkanSampler;
    class VulkanPipeline;
    class VulkanDescriptorLayout;
    class VulkanSwapChain;
    class VulkanRenderPass;

    class VulkanUtils
    {
    public:
        VulkanUtils(
            VulkanDevice *vulkanDevice,
            VulkanCommandPool *vulkanCommandPool,
            VkQueue graphicsQueue,
            VkQueue presentQueue);
        ~VulkanUtils();
        bool setup();

        int64 findMemoryType(uint32 typeFilter, uint64 properties) noexcept;
        bool createBuffer(uint64 size, uint32 usage, uint32 properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
        bool copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, uint64 size);
        void transitionImageLayout(VkImage image, uint64 format, uint64 oldLayout, uint64 newLayout);
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32 width, uint32 height);
        void destroyPipelines();
        bool rebuildPipelines(uint32 width, uint32 height,
                              VulkanSwapChain *vulkanSwapChain,
                              VulkanRenderPass *vulkanRenderPass);

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

        inline VulkanSampler *getSampler()
        {
            return vulkanSampler.get();
        }

        inline VulkanCommandBuffer *getCurrentCommandBuffer()
        {
            return currentCommandBuffer;
        }

        inline void setCurrentCommandBuffer(VulkanCommandBuffer *newCommandBuffer)
        {
            currentCommandBuffer = newCommandBuffer;
        }

        inline VulkanDescriptorPool *getDescriptorPool()
        {
            return vulkanDescriptorPool.get();
        }

        inline VulkanPipelineColored *getPipelineColored()
        {
            return vulkanPipelineColored.get();
        }

        inline VulkanPipeline *getPipelineTextured()
        {
            return vulkanPipelineTextured.get();
        }

        inline VulkanPipeline *getCurrentPipeline()
        {
            return currentPipeline;
        }

        inline void enablePipelineColored()
        {
            currentPipeline = vulkanPipelineColored.get();
            currentCommandBuffer->bindPipeline(currentPipeline);
        }

        inline void enablePipelineTextured()
        {
            currentPipeline = vulkanPipelineTextured.get();
            currentCommandBuffer->bindPipeline(currentPipeline);
        }

        inline void enablePipelineByType(ModelDataType dataType)
        {
            if (dataType == ModelDataType::VertexColoredInd16 || dataType == ModelDataType::VertexColoredInd32)
                enablePipelineColored();
            else if (dataType == ModelDataType::VertexTexturedInd16 || dataType == ModelDataType::VertexTexturedInd32)
                enablePipelineTextured();
        }

    protected:
        std::unique_ptr<VulkanDescriptorPool> vulkanDescriptorPool;
        std::unique_ptr<VulkanSampler> vulkanSampler;

        // pipelines
        std::unique_ptr<VulkanPipelineColored> vulkanPipelineColored;
        std::unique_ptr<VulkanPipelineTextured> vulkanPipelineTextured;
        VulkanPipeline *currentPipeline = nullptr;

        VulkanDevice *vulkanDevice = nullptr;
        VulkanCommandPool *vulkanCommandPool = nullptr;
        VulkanCommandBuffer *currentCommandBuffer = nullptr;

        VkDevice device;
        VkPhysicalDevice physicalDevice;
        VkQueue graphicsQueue = nullptr;
        VkQueue presentQueue = nullptr;

        bool anisotropySupported = false;

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    };
};