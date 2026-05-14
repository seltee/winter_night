#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/pipelines/vulkanPipeline.h"
#include "features/renderer/vulkan/vulkanDescriptorPool.h"
#include "features/renderer/vulkan/vulkanObjectBuffers.h"
#include <memory>

#include "core/core.h"
#include "core/api.h"

namespace wne
{
    class VulkanShader;
    class VulkanRenderPass;
    class VulkanDevice;
    class VulkanObjectBuffers;

    class WNE_API VulkanPipelineColored : public VulkanPipeline
    {
    public:
        VulkanPipelineColored(VulkanDevice *vulkanDevice);
        ~VulkanPipelineColored();
        VkPipeline getGraphicsPipeline() override final;
        VkPipelineLayout getPipelineLayout() override final;
        VkDescriptorSetLayout getDescriptorSetLayoutPipeline() override final;
        VkDescriptorSet getDescriptorSet() override final;

        bool setup(
            VulkanRenderPass *renderPass,
            VulkanDescriptorPool *vulkanDescriptorPool,
            VulkanObjectBuffers *vulkanObjectBuffers,
            bool depthWriteStage);
        void updateDescriptorSet(VulkanObjectBuffers *vulkanObjectBuffers);

    protected:
        bool createLayouts();

        VkDescriptorSetLayout descriptorSetLayoutPipeline = nullptr;

        std::unique_ptr<VulkanShader> shader;
        VkPipelineLayout pipelineLayout = nullptr;
        VkPipeline graphicsPipeline = nullptr;
        VkDescriptorSet descriptorSet = nullptr;
    };
}