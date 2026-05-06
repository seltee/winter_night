#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanUtils.h"

namespace wne
{
    class VulkanSampler
    {
    public:
        VulkanSampler(VulkanUtils *vulkanUtils);
        ~VulkanSampler();
        bool setup();

        inline VkSampler getTextureSampler()
        {
            return textureSampler;
        }

    protected:
        VkSampler textureSampler = nullptr;
        VulkanUtils *vulkanUtils = nullptr;
    };
};
