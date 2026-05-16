#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "core/core.h"
#include "core/api.h"

namespace wne
{
    class VulkanShader;
    class VulkanRenderPass;
    class VulkanDevice;
    class VulkanDescriptorSetLayout;

    class WNE_API VulkanPipeline
    {
    public:
        virtual ~VulkanPipeline();
        virtual VkPipeline getGraphicsPipeline();
        virtual VkPipelineLayout getPipelineLayout();
        virtual VulkanDescriptorSetLayout *getDescriptorSetLayoutPipeline();
        virtual VulkanDescriptorSetLayout *getDescriptorSetLayoutSampler();

    protected:
        VulkanPipeline(VulkanDevice *vulkanDevice);
        VulkanDevice *vulkanDevice;
    };
}