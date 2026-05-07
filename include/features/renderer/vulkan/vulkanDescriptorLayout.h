#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "core/math.h"

namespace wne
{
    class VulkanDevice;

    class VulkanDescriptorLayout
    {
    public:
        VulkanDescriptorLayout(VulkanDevice *vulkanDevice);
        ~VulkanDescriptorLayout();
        bool setup();

        inline VkDescriptorSetLayout getDescriptorLayout()
        {
            return descriptorSetLayout;
        }

    protected:
        VkDescriptorSetLayout descriptorSetLayout = nullptr;
        VulkanDevice *vulkanDevice;
    };
};