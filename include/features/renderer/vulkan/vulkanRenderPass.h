#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include <memory>
namespace wne
{
    class VulkanDepthBuffer;
    class VulkanDevice;
    class VulkanUtils;

    class VulkanRenderPass
    {
    public:
        VulkanRenderPass(VulkanUtils *vulkanUtils);
        ~VulkanRenderPass();

        bool setupColor(VulkanFormat imageFormat, uint64 MSAASampleCountBit);
        bool setupDepth(bool isSampled, uint64 MSAASampleCountBit);
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