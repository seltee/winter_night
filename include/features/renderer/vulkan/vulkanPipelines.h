#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#include "features/renderer/vulkan/vulkanSwapChain.h"
#include "features/renderer/vulkan/vulkanRenderPass.h"
#include "features/renderer/vulkan/vulkanDescriptorPool.h"
#include "features/renderer/vulkan/vulkanObjectBuffers.h"
#include "features/renderer/vulkan/vulkanCommandBuffer.h"
#include "features/renderer/vulkan/pipelines/vulkanPipeline.h"
#include "features/renderer/vulkan/pipelines/vulkanPipelineColored.h"
#include "features/renderer/vulkan/pipelines/vulkanPipelineTextured.h"
#include <memory>

namespace wne
{
    class VulkanPipelines
    {
    public:
        VulkanPipelines(VulkanDevice *vulkanDevice);

        void reset();
        bool build(
            VulkanSwapChain *vulkanSwapChain,
            VulkanRenderPass *vulkanRenderPass,
            VulkanDescriptorPool *vulkanDescriptorPool,
            VulkanObjectBuffers *vulkanObjectBuffers);

        void enablePipelineColored(VulkanCommandBuffer *commandBuffer, bool isDepthRendering);
        void enablePipelineTextured(VulkanCommandBuffer *commandBuffer, bool isDepthRendering);

        inline VulkanPipeline *getCurrentPipeline()
        {
            return currentPipeline;
        }

    protected:
        VulkanDevice *vulkanDevice = nullptr;

        std::unique_ptr<VulkanPipelineColored> vulkanPipelineColoredDepth;
        std::unique_ptr<VulkanPipelineTextured> vulkanPipelineTexturedDepth;

        std::unique_ptr<VulkanPipelineColored> vulkanPipelineColoredColor;
        std::unique_ptr<VulkanPipelineTextured> vulkanPipelineTexturedColor;

        VulkanPipeline *currentPipeline = nullptr;
    };
};
