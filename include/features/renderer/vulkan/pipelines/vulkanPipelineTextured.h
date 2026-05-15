#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/pipelines/vulkanPipeline.h"
#include "features/renderer/vulkan/pipelines/vulkanDescriptorSetLayout.h"
#include "features/renderer/vulkan/vulkanDescriptorPool.h"
#include "features/renderer/vulkan/vulkanObjectBuffers.h"
#include <memory>

#include "core/core.h"
#include "core/api.h"

namespace wne
{
    class VulkanShader;
    class VulkanDescriptorLayout;
    class VulkanRenderPass;
    class VulkanDevice;

    class WNE_API VulkanPipelineTextured : public VulkanPipeline
    {
    public:
        VulkanPipelineTextured(VulkanDevice *vulkanDevice);
        ~VulkanPipelineTextured();
        VkPipeline getGraphicsPipeline() override final;
        VkPipelineLayout getPipelineLayout() override final;

        bool setupColor(
            VulkanRenderPass *renderPass,
            VulkanDescriptorPool *vulkanDescriptorPool,
            VulkanObjectBuffers *vulkanObjectBuffers);

        bool setupDepth(
            VulkanRenderPass *depthPass,
            VulkanDescriptorPool *vulkanDescriptorPool,
            VulkanObjectBuffers *vulkanObjectBuffers);

        void updateDescriptorSetColor(VulkanObjectBuffers *vulkanObjectBuffers);
        void updateDescriptorSetDepth(VulkanObjectBuffers *vulkanObjectBuffers);

        VkDescriptorSetLayout getDescriptorSetLayoutPipeline() override final;
        VkDescriptorSetLayout getDescriptorSetLayoutSampler() override final;
        VkDescriptorSet getDescriptorSet() override final;

    protected:
        bool buildShaderColor();
        bool buildShaderDepth();
        bool buildPipeline(
            uint32 stageAmount,
            bool enableColorBlending,
            bool enableDepthWrite,
            bool enableSampler,
            VulkanRenderPass *renderPass,
            VulkanDescriptorPool *vulkanDescriptorPool,
            VulkanObjectBuffers *vulkanObjectBuffers);

        std::unique_ptr<VulkanDescriptorSetLayout> descriptorSetLayoutPipeline;
        std::unique_ptr<VulkanDescriptorSetLayout> descriptorSetLayoutSampler;

        std::unique_ptr<VulkanShader> shader;
        VkPipelineLayout pipelineLayout = nullptr;
        VkPipeline graphicsPipeline = nullptr;
        VkDescriptorSet descriptorSet = nullptr;
    };
};
