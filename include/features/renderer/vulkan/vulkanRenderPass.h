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

        bool setup(uint16 width, uint16 height, VulkanFormat imageFormat);
        inline VkRenderPass getRenderPass()
        {
            return renderPass;
        }

        inline VulkanDepthBuffer *getDepthBuffer()
        {
            return vulkanDepthBuffer.get();
        }

        inline uint16 getWidth()
        {
            return width;
        }

        inline uint16 getHeight()
        {
            return height;
        }

    protected:
        uint16 width = 0, height = 0;

        VkRenderPass renderPass = nullptr;

        VulkanUtils *vulkanUtils = nullptr;
        VulkanDevice *vulkanDevice = nullptr;

        std::unique_ptr<VulkanDepthBuffer> vulkanDepthBuffer;
    };
}