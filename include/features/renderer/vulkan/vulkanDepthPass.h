#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include <memory>
namespace wne
{
    class VulkanDepthBuffer;
    class VulkanDevice;
    class VulkanUtils;

    class VulkanDepthPass
    {
    public:
        VulkanDepthPass(VulkanUtils *vulkanUtils);
        ~VulkanDepthPass();

        bool setup(bool isSampled);
        inline VkRenderPass getRenderPass()
        {
            return renderPass;
        }

    protected:
        VulkanFormat findDepthFormat(bool isSampled);

        VkRenderPass renderPass = nullptr;

        VulkanUtils *vulkanUtils = nullptr;
        VulkanDevice *vulkanDevice = nullptr;
    };
}