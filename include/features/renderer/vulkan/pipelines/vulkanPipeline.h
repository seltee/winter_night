#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "core/core.h"
#include "core/api.h"

namespace wne
{
    class VulkanShader;
    class VulkanRenderPass;
    class VulkanDevice;

    class WNE_API VulkanPipeline
    {
    public:
        virtual ~VulkanPipeline();
        virtual VkDescriptorSetLayout getDescriptorSetLayout();

        inline const VkPipeline getGraphicsPipeline()
        {
            return graphicsPipeline;
        }

        inline const VkPipelineLayout getPipelineLayout()
        {
            return pipelineLayout;
        }

    protected:
        VulkanPipeline(VulkanDevice *vulkanDevice);

        VulkanDevice *vulkanDevice;

        VkPipelineLayout pipelineLayout = nullptr;
        VkPipeline graphicsPipeline = nullptr;
    };
}