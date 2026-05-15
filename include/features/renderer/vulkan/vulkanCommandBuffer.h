#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"

namespace wne
{
    class VulkanDevice;
    class VulkanRenderPass;
    class VulkanFrameBuffer;
    class VulkanCommandPool;
    class VulkanPipeline;

    class VulkanCommandBuffer
    {
    public:
        VulkanCommandBuffer(VulkanDevice *vulkanDevice,
                            VulkanCommandPool *commandPool);
        ~VulkanCommandBuffer();

        bool setup();

        void recordCommandBuffer();
        void beginRenderPass(VulkanRenderPass *renderPass, VkFramebuffer frameBuffer, uint16 width, uint16 height);
        void beginDepthPass(VulkanRenderPass *depthPass, VkFramebuffer frameBuffer, uint16 width, uint16 height);
        void endPass();
        void endCommandBuffer();
        void bindPipeline(VulkanPipeline *vulkanPipeline);
        void resetBuffer();

        inline VkCommandBuffer getCommandBuffer()
        {
            return commandBuffer;
        }

    protected:
        uint16 passWidth, passHeight;

        VkPhysicalDevice physicalDevice;
        VkDevice device;
        VkCommandBuffer commandBuffer;

        VulkanCommandPool *commandPool;
    };
}