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

        bool setup(uint16 width, uint16 height, VulkanFormat swapChainImageFormat);
        inline VkRenderPass getRenderPass()
        {
            return renderPass;
        }

        inline VulkanDepthBuffer *getDepthBuffer()
        {
            return vulkanDepthBuffer.get();
        }

    protected:
        VkRenderPass renderPass = nullptr;

        VulkanUtils *vulkanUtils = nullptr;
        VulkanDevice *vulkanDevice = nullptr;

        std::unique_ptr<VulkanDepthBuffer> vulkanDepthBuffer;
    };
}