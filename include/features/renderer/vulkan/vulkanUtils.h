#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#include "features/renderer/vulkan/vulkanCommandBuffer.h"
#include "features/renderer/vulkan/vulkanPipelines.h"
#include "features/renderer/vulkan/vulkanShader.h"
#include "features/renderer/vulkan/vulkanShadowMaps.h"
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
    class VulkanObjectBuffers;
    class VulkanDescriptorSets;

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
        VulkanFormat findSupportedFormat(const std::vector<VulkanFormat> &candidates, VulkanImageTiling tiling, VulkanFormatFeatureFlags features) noexcept;

        bool createBuffer(uint64 size, uint32 usage, uint32 properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
        bool copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, uint64 size);
        void transitionImageLayout(VkImage image, uint64 format, uint64 oldLayout, uint64 newLayout);
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32 width, uint32 height);
        void destroyPipelines();
        bool rebuildPipelines(VulkanSwapChain *vulkanSwapChain, VulkanRenderPass *vulkanRenderPass, VulkanRenderPass *vulkanDepthPass);
        void updatePipelineShadowMaps();

        bool createImage(
            uint16 width,
            uint16 height,
            VulkanFormat format,
            VulkanImageTiling tiling,
            VulkanImageUsageFlags usage,
            VulkanMemoryPropertyFlagBits memoryFlags,
            VkImage *pImage,
            VkDeviceMemory *pMemory);

        bool createImageView(VkImage image, VulkanFormat format, VulkanImageAspectFlags aspectFlags, VkImageView *imageView);

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);

        inline void swapSets()
        {
            vulkanObjectBuffers->swap();
        }

        inline VulkanTexture *getDummyTexture()
        {
            return dummyTexture.get();
        }

        inline VulkanDevice *getVulkanDevice()
        {
            return vulkanDevice;
        }

        inline VulkanCommandPool *getVulkanCommandPool()
        {
            return vulkanCommandPool;
        }

        inline VulkanObjectBuffers *getObjectBuffers()
        {
            return vulkanObjectBuffers.get();
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

        inline VulkanPipeline *getCurrentPipeline()
        {
            return vulkanPipelines->getCurrentPipeline();
        }

        inline void enablePipelineByType(ModelDataType dataType, bool isDepthRendering)
        {
            if (dataType == ModelDataType::VertexColoredInd16 || dataType == ModelDataType::VertexColoredInd32)
                vulkanPipelines->enablePipelineColored(currentCommandBuffer, isDepthRendering);
            else if (dataType == ModelDataType::VertexTexturedInd16 || dataType == ModelDataType::VertexTexturedInd32)
                vulkanPipelines->enablePipelineTextured(currentCommandBuffer, isDepthRendering);
        }

        inline void enablePipelineShadowByType(ModelDataType dataType)
        {
            vulkanPipelines->enablePipelineTexturedShadowDepth(currentCommandBuffer);
        }

        inline void enablePipelineAtmosphere()
        {
            vulkanPipelines->enablePipelineAtmosphere(currentCommandBuffer);
        }

        inline VulkanRenderPass *getCurrentRenderPass()
        {
            return vulkanRenderPass;
        }

        inline VulkanRenderPass *getCurrentDepthPass()
        {
            return vulkanDepthPass;
        }

        inline VulkanShadowMaps *getShadowMaps()
        {
            return vulkanShadowMaps.get();
        }

        inline VkQueue getGraphicsQueue()
        {
            return graphicsQueue;
        }

        inline VkQueue getPresentQueue()
        {
            return presentQueue;
        }

        inline VulkanDescriptorSets *getDescriptorSets()
        {
            return vulkanPipelines->getDescriptorSets();
        }

        inline void setRadianceMap(VulkanTexture *radianceMap)
        {
            vulkanPipelines->getDescriptorSets()->updateRadianceMap(radianceMap ? radianceMap : dummyTexture.get(), vulkanSampler.get());
        }

    protected:
        std::unique_ptr<VulkanDescriptorPool> vulkanDescriptorPool;
        std::unique_ptr<VulkanObjectBuffers> vulkanObjectBuffers;
        std::unique_ptr<VulkanSampler> vulkanSampler;
        std::unique_ptr<VulkanShadowMaps> vulkanShadowMaps;

        // pipelines
        std::unique_ptr<VulkanPipelines> vulkanPipelines;

        VulkanDevice *vulkanDevice = nullptr;
        VulkanCommandPool *vulkanCommandPool = nullptr;
        VulkanCommandBuffer *currentCommandBuffer = nullptr;

        VkDevice device;
        VkPhysicalDevice physicalDevice;
        VkQueue graphicsQueue = nullptr;
        VkQueue presentQueue = nullptr;

        bool anisotropySupported = false;

        VulkanRenderPass *vulkanRenderPass = nullptr;
        VulkanRenderPass *vulkanDepthPass = nullptr;

        std::unique_ptr<VulkanTexture> dummyTexture;
    };
};