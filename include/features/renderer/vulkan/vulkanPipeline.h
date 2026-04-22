#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanShader.h"
#include "features/renderer/vulkan/vulkanRenderPass.h"

namespace WNE
{
    class VulkanPipeline
    {
    public:
        ~VulkanPipeline();
        bool setup(int width, int height, VkExtent2D *swapChainExtent, VkDevice device, VulkanRenderPass *renderPass, VulkanShader *shader);

        inline const VkPipeline getGraphicsPipeline()
        {
            return graphicsPipeline;
        }

    protected:
        VkPipelineLayout pipelineLayout = nullptr;
        VkPipeline graphicsPipeline = nullptr;
        VkDevice device = nullptr;
    };
}