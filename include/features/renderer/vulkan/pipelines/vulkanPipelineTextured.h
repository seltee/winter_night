#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/pipelines/vulkanPipeline.h"
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

        bool setup(VkExtent2D *swapChainExtent, VulkanRenderPass *renderPass);

        VkDescriptorSetLayout getDescriptorSetLayout() override final;

    protected:
        VkDescriptorSetLayout descriptorSetLayout = nullptr;
        std::unique_ptr<VulkanShader> shader;
        VkPipelineLayout pipelineLayout = nullptr;
        VkPipeline graphicsPipeline = nullptr;
    };
}