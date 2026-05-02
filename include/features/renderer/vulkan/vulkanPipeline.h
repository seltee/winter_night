#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "core/core.h"

namespace wne
{
    class VulkanShader;
    class VulkanDescriptorLayout;
    class VulkanRenderPass;

    class VulkanPipeline
    {
    public:
        ~VulkanPipeline();
        bool setup(
            uint width,
            uint height,
            VkExtent2D *swapChainExtent,
            VkDevice device,
            VulkanRenderPass *renderPass,
            VulkanShader *shader,
            VulkanDescriptorLayout *vulkanDescriptorLayout);

        inline const VkPipeline getGraphicsPipeline()
        {
            return graphicsPipeline;
        }

        inline const VkPipelineLayout getPipelineLayout()
        {
            return pipelineLayout;
        }

    protected:
        VkPipelineLayout pipelineLayout = nullptr;
        VkPipeline graphicsPipeline = nullptr;
        VkDevice device = nullptr;
    };
}