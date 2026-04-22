#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"

namespace WNE
{
    class VulkanRenderPass
    {
    public:
        VulkanRenderPass();
        ~VulkanRenderPass();

        bool setup(int swapChainImageFormat, VkDevice device);
        inline VkRenderPass getRenderPass()
        {
            return renderPass;
        }

    protected:
        VkRenderPass renderPass = nullptr;
        VkDevice device = nullptr;
    };
}