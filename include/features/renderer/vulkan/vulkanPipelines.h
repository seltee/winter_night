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
#include "features/renderer/vulkan/pipelines/vulkanDescriptorSets.h"
#include "core/core.h"
#include <iostream>
#include <memory>

namespace wne
{
    class VulkanShadowMaps;
    class VulkanSampler;
    class VulkanDescriptorSets;

    class VulkanPipelines
    {
    public:
        VulkanPipelines(VulkanDevice *vulkanDevice);

        void reset();
        bool build(
            VulkanSwapChain *vulkanSwapChain,
            VulkanRenderPass *vulkanRenderPass,
            VulkanRenderPass *VulkanDepthPass,
            VulkanRenderPass *VulkanShadowDepthPass,
            VulkanDescriptorPool *vulkanDescriptorPool,
            VulkanObjectBuffers *vulkanObjectBuffers,
            uint64 MSAASampleCountBit);

        void enablePipelineColored(VulkanCommandBuffer *commandBuffer, bool isDepthRendering);
        void enablePipelineTexturedDepth(VulkanCommandBuffer *commandBuffer, bool isMasked);
        void enablePipelineTexturedShadowDepth(VulkanCommandBuffer *commandBuffer, bool isMasked, bool isDoubleSided);
        void enablePipelineTextured(VulkanCommandBuffer *commandBuffer, bool isLightEnabled, ColorBlending blending);
        void enablePipelineAtmosphere(VulkanCommandBuffer *commandBuffer);

        inline void updatePipelineShadowMaps(VulkanShadowMaps *shadowMaps, VulkanSampler *sampler)
        {
            vulkanDescriptorSets->updateShadowMap(shadowMaps, sampler);
        }

        inline void setCurrentPipeline(VulkanPipeline *newCurrentPipeline)
        {
            currentPipeline = newCurrentPipeline;
        }

        inline VulkanPipeline *getCurrentPipeline()
        {
            return currentPipeline;
        }

        inline VulkanDescriptorSets *getDescriptorSets()
        {
            return vulkanDescriptorSets.get();
        }

    protected:
        VulkanDevice *vulkanDevice = nullptr;

        std::unique_ptr<VulkanPipelineColored> vulkanPipelineColoredColor;
        std::unique_ptr<VulkanPipelineColored> vulkanPipelineColoredDepth;

        std::unique_ptr<VulkanPipelineTextured> vulkanPipelineTexturedSingleSideShadowDepth;
        std::unique_ptr<VulkanPipelineTextured> vulkanPipelineTexturedSingleSideMaskedShadowDepth;
        std::unique_ptr<VulkanPipelineTextured> vulkanPipelineTexturedDoubleSideShadowDepth;
        std::unique_ptr<VulkanPipelineTextured> vulkanPipelineTexturedDoubleSideMaskedShadowDepth;
        std::unique_ptr<VulkanPipelineTextured> vulkanPipelineTexturedDepth;
        std::unique_ptr<VulkanPipelineTextured> vulkanPipelineTexturedMaskedDepth;

        std::unique_ptr<VulkanPipelineTextured> vulkanPipelineTextured[val(LightState::Total)][val(ColorBlending::Total)];

        std::unique_ptr<VulkanPipelineTextured> vulkanPipelineAtmosphereColor;

        std::unique_ptr<VulkanDescriptorSets> vulkanDescriptorSets;

        VulkanPipeline *currentPipeline = nullptr;
    };
};
