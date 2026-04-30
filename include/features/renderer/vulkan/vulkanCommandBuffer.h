#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanQueueFamilies.h"
#include "features/renderer/vulkan/vulkanRenderPass.h"
#include "features/renderer/vulkan/vulkanFrameBuffer.h"
#include "features/renderer/vulkan/vulkanPipeline.h"
#include "features/renderer/vulkan/vulkanCommandPool.h"
#include "features/renderer/vulkan/vulkanDevice.h"

namespace WNE
{
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