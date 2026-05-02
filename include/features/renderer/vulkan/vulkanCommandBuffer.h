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
        VulkanCommandBuffer(VulkanDevice *vkDevice,
                            VulkanRenderPass *renderPass,
                            VulkanFrameBuffer *frameBuffer,
                            VkExtent2D *swapChainExtent,
                            VulkanCommandPool *commandPool);
        ~VulkanCommandBuffer();

        bool setup(VkSurfaceKHR surface);

        void recordCommandBuffer(uint32_t imageIndex);
        void bindPipeline(VulkanPipeline *vulkanPipeline);
        void endRenderPass();
        void resetBuffer();

        inline VkCommandBuffer getCommandBuffer()
        {
            return commandBuffer;
        }

    protected:
        VkPhysicalDevice physicalDevice;
        VkDevice device;
        VkExtent2D *swapChainExtent;
        VkCommandBuffer commandBuffer;

        VulkanFrameBuffer *frameBuffer;
        VulkanRenderPass *renderPass;
        VulkanCommandPool *commandPool;
    };
}