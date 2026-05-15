#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"

namespace wne
{
    class VulkanDevice;
    class VulkanDescriptorSetLayout
    {
    public:
        VulkanDescriptorSetLayout(VulkanDevice *vulkanDevice);
        ~VulkanDescriptorSetLayout();
        bool setupTexturedDepth();
        bool setupTexturedColor();
        bool setupSampler();

        inline VkDescriptorSetLayout getDescriptorSetLayout()
        {
            return descriptorSetLayout;
        }

    protected:
        VkDescriptorSetLayout descriptorSetLayout = nullptr;
        VulkanDevice *vulkanDevice = nullptr;
    };
};
