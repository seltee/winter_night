#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#include "features/renderer/vulkan/vulkanSwapChain.h"
#include "features/renderer/vulkan/vulkanRenderPass.h"
#include "features/renderer/vulkan/vulkanDescriptorPool.h"
#include "features/renderer/vulkan/vulkanObjectBuffers.h"
#include "features/renderer/vulkan/vulkanCommandBuffer.h"
#include "features/renderer/vulkan/pipelines/vulkanPipeline.h"
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
        VulkanPipelines();

        inline void setCurrentPipeline(VulkanPipeline *newCurrentPipeline)
        {
            this->currentPipeline = newCurrentPipeline;
        }

        inline VulkanPipeline *getCurrentPipeline()
        {
            return currentPipeline;
        }

    protected:
        VulkanPipeline *currentPipeline = nullptr;
    };
};
