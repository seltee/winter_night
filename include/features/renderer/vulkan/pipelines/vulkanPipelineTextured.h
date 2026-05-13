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

        bool setup(
            VkExtent2D *swapChainExtent,
            VulkanRenderPass *renderPass,
            VulkanDescriptorPool *vulkanDescriptorPool,
            VulkanObjectBuffers *vulkanObjectBuffers, 
            bool depthWriteStage);
        void updateDescriptorSet(VulkanObjectBuffers *vulkanObjectBuffers);

        VkDescriptorSetLayout getDescriptorSetLayoutPipeline() override final;
        VkDescriptorSetLayout getDescriptorSetLayoutSampler() override final;
        VkDescriptorSet getDescriptorSet() override final;

    protected:
        bool createLayouts();

        VkDescriptorSetLayout descriptorSetLayoutPipeline = nullptr;
        VkDescriptorSetLayout descriptorSetLayoutSampler = nullptr;
        std::unique_ptr<VulkanShader> shader;
        VkPipelineLayout pipelineLayout = nullptr;
        VkPipeline graphicsPipeline = nullptr;
        VkDescriptorSet descriptorSet = nullptr;
    };
}