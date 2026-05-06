#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include <vector>

namespace wne
{
    class VulkanDevice;
    class VulkanDescriptorPool;
    class VulkanDescriptorLayout;

    class VulkanDescriptorSet
    {
    public:
        VulkanDescriptorSet(VulkanDevice *vulkanDevice);
        ~VulkanDescriptorSet();
        bool setup(VulkanDescriptorPool *vulkanDescriptorPool, VulkanDescriptorLayout *vulkanDescriptorLayout);

        inline VkDescriptorSet getDescriptorSet()
        {
            return descriptorSet;
        }

    protected:
        VulkanDevice *vulkanDevice = nullptr;
        VkDescriptorSet descriptorSet;
    };
};