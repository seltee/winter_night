#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanObjectBuffers.h"

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
            VulkanPipelineTextured *pipelineTexturedDepth,
            VulkanPipelineTextured *pipelineTexturedColor);

        void updateShadowMap(VulkanShadowMaps *shadowMaps, VulkanSampler *sampler);

        inline VkDescriptorSet getDescriptorSetColoredColor()
        {
            return descriptorSetColoredColor[vulkanObjectBuffers->getFrameInFlight()];
        }
        inline VkDescriptorSet getDescriptorSetColoredDepth()
        {
            return descriptorSetColoredDepth[vulkanObjectBuffers->getFrameInFlight()];
        }
        inline VkDescriptorSet getDescriptorSetTexturedColor()
        {
            return descriptorSetTexturedColor[vulkanObjectBuffers->getFrameInFlight()];
        }
        inline VkDescriptorSet getDescriptorSetTexturedDepth()
        {
            return descriptorSetTexturedDepth[vulkanObjectBuffers->getFrameInFlight()];
        }

    protected:
        bool initDescriptorSetColoredDepth(uint frame, VkDescriptorSet *descriptorSet);

        bool initDescriptorSetColoredColor(uint frame, VkDescriptorSet *descriptorSet);

        bool initDescriptorSetTexturedDepth(
            uint frame, 
            VkDescriptorSet *descriptorSet,
            VulkanDescriptorSetLayout *descriptorSetLayoutPipeline);

        bool initDescriptorSetTexturedColor(
            uint frame, 
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
