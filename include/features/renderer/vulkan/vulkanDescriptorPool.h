#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"

namespace wne
{
    class VulkanDevice;

    class VulkanDescriptorPool
    {
    public:
        VulkanDescriptorPool(VulkanDevice *vulkanDevice);
        ~VulkanDescriptorPool();
        bool setup(int maxFrames);

        inline VkDescriptorPool getDescriptorPool()
        {
            return descriptorPool;
        }

    protected:
        VkDescriptorPool descriptorPool;

        VulkanDevice *vulkanDevice = nullptr;
    };
};