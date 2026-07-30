#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanObjectBuffers.h"

#include <vector>

namespace wne
{
    class VulkanDescriptorPool;
    class VulkanDevice;
    class VulkanPipelineUniversal;
    class VulkanDescriptorSetLayout;
    class VulkanObjectBuffers;
    class VulkanShadowMaps;
    class VulkanSampler;
    class VulkanUtils;

    class VulkanDescriptorSets
    {
    public:
        VulkanDescriptorSets(VulkanUtils *vulkanUtils);
        ~VulkanDescriptorSets();

        bool setup();
        bool setupColor();
        bool setupDepth();

        void updateShadowMap(VulkanShadowMaps *shadowMaps, VulkanSampler *sampler);
        void updateRadianceMap(VulkanTexture *texture, VulkanSampler *sampler);

        inline VkDescriptorSet getDescriptorSetTexturedColor()
        {
            return descriptorSetColor[vulkanObjectBuffers->getFrameInFlight()];
        }
        inline VkDescriptorSet getDescriptorSetTexturedDepth()
        {
            return descriptorSetDepth[vulkanObjectBuffers->getFrameInFlight()];
        }

    protected:
        bool initDescriptorSetTexturedDepth(
            uint frame,
            VkDescriptorSet *descriptorSet,
            VulkanDescriptorSetLayout *descriptorSetLayoutPipeline);

        bool initDescriptorSetTexturedColor(
            uint frame,
            VkDescriptorSet *descriptorSet,
            VulkanDescriptorSetLayout *descriptorSetLayoutPipeline,
            VulkanDescriptorSetLayout *descriptorSetLayoutSampler);

        std::vector<VkDescriptorSet> descriptorSetColor;
        std::vector<VkDescriptorSet> descriptorSetDepth;

        VulkanDescriptorPool *vulkanDescriptorPool;
        VulkanDevice *vulkanDevice;
        VulkanObjectBuffers *vulkanObjectBuffers;
        VulkanUtils *vulkanUtils;
    };
};
