#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include <vector>

namespace wne
{
    class VulkanDevice;
    class VulkanDescriptorPool;
    class VulkanDescriptorLayout;

    class VulkanDescriptorSets
    {
    public:
        VulkanDescriptorSets(VulkanDevice *vulkanDevice);
        ~VulkanDescriptorSets();
        bool setup(int maxFrames, VulkanDescriptorPool *vulkanDescriptorPool, VulkanDescriptorLayout *vulkanDescriptorLayout);

        inline VkDescriptorSet getDescriptorSet(int index)
        {
            return descriptorSets[index];
        }

    protected:
        VulkanDevice *vulkanDevice = nullptr;
        std::vector<VkDescriptorSet> descriptorSets;
    };
};