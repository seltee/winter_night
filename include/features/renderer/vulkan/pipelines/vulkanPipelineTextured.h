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

        bool setup(
            uint width,
            uint height,
            VkExtent2D *swapChainExtent,
            VulkanRenderPass *renderPass);

        VkDescriptorSetLayout getDescriptorSetLayout() override final;

        inline const VkPipeline getGraphicsPipeline()
        {
            return graphicsPipeline;
        }

        inline const VkPipelineLayout getPipelineLayout()
        {
            return pipelineLayout;
        }

    protected:
        VkDescriptorSetLayout descriptorSetLayout = nullptr;
        std::unique_ptr<VulkanShader> shader;
    };
}