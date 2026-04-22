#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanQueueFamilies.h"
#include "features/renderer/vulkan/vulkanRenderPass.h"
#include "features/renderer/vulkan/vulkanFrameBuffer.h"
#include "features/renderer/vulkan/vulkanPipeline.h"

namespace WNE
{
    class VulkanCommandBuffer
    {
    public:
        VulkanCommandBuffer(VkDevice device,
                            VkPhysicalDevice physicalDevice,
                            VulkanRenderPass *renderPass,
                            VulkanFrameBuffer *frameBuffer,
                            VkExtent2D *swapChainExtent);
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
        VulkanRenderPass *renderPass;
        VulkanFrameBuffer *frameBuffer;
        VkExtent2D *swapChainExtent;
        VkCommandPool commandPool;
        VkCommandBuffer commandBuffer;
    };
}