#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include <vector>

namespace wne
{
    class VulkanDescriptorPool;
    class VulkanDevice;
    class VulkanPipelineTextured;
    class VulkanDescriptorSetLayout;
    class VulkanObjectBuffers;
    class VulkanShadowMaps;
    class VulkanSampler;

    class VulkanDescriptorSets
    {
    public:
        VulkanDescriptorSets(VulkanDevice *vulkanDevice, VulkanDescriptorPool *vulkanDescriptorPool, VulkanObjectBuffers *vulkanObjectBuffers);
        ~VulkanDescriptorSets();

        bool setup(
            uint maxFramesInFlight,
            VulkanPipelineTextured *pipelineTexturedDepth,
            VulkanPipelineTextured *pipelineTexturedColor);
        void swap();

        void updateShadowMap(VulkanShadowMaps *shadowMaps, VulkanSampler *sampler);

        inline VkDescriptorSet getDescriptorSetColoredColor()
        {
            return descriptorSetColoredColor[currentInFlight];
        }
        inline VkDescriptorSet getDescriptorSetColoredDepth()
        {
            return descriptorSetColoredDepth[currentInFlight];
        }
        inline VkDescriptorSet getDescriptorSetTexturedColor()
        {
            return descriptorSetTexturedColor[currentInFlight];
        }
        inline VkDescriptorSet getDescriptorSetTexturedDepth()
        {
            return descriptorSetTexturedDepth[currentInFlight];
        }

    protected:
        uint maxFramesInFlight = 0;
        uint currentInFlight = 0;

        bool initDescriptorSetColoredDepth(VkDescriptorSet *descriptorSet);

        bool initDescriptorSetColoredColor(VkDescriptorSet *descriptorSet);

        bool initDescriptorSetTexturedDepth(
            VkDescriptorSet *descriptorSet,
            VulkanDescriptorSetLayout *descriptorSetLayoutPipeline);

        bool initDescriptorSetTexturedColor(
            VkDescriptorSet *descriptorSet,
            VulkanDescriptorSetLayout *descriptorSetLayoutPipeline,
            VulkanDescriptorSetLayout *descriptorSetLayoutSampler);

        std::vector<VkDescriptorSet> descriptorSetColoredColor;
        std::vector<VkDescriptorSet> descriptorSetColoredDepth;
        std::vector<VkDescriptorSet> descriptorSetTexturedColor;
        std::vector<VkDescriptorSet> descriptorSetTexturedDepth;

        VulkanDescriptorPool *vulkanDescriptorPool;
        VulkanDevice *vulkanDevice;
        VulkanObjectBuffers *vulkanObjectBuffers;
    };
};
