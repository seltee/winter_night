#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanCommandBuffer.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#include "features/renderer/vulkan/vulkanRenderPass.h"
#include "features/renderer/vulkan/vulkanFrameBuffer.h"
#include "features/renderer/vulkan/vulkanSwapChain.h"
#include <vector>

namespace WNE
{
    class VulkanFrame
    {
    public:
        VulkanFrame(VulkanDevice *vulkanDevice, VulkanSwapChain *swapChain);
        ~VulkanFrame();
        bool setup(VulkanRenderPass *renderPass, VulkanFrameBuffer *frameBuffer, VulkanCommandPool *commandPool);

        void startFrame(VulkanPipeline *pipeline);
        void finishFrame(VkQueue graphicsQueue, VkQueue presentQueue);

        inline VulkanCommandBuffer *getCommandBuffer()
        {
            return commandBuffer;
        }

    protected:
        VkSemaphore imageAvailableSemaphore = nullptr;
        VkSemaphore renderFinishedSemaphore = nullptr;
        VkFence inFlightFence = nullptr;
        VkDevice device = nullptr;
        VkPhysicalDevice physicalDevice = nullptr;
        VkSurfaceKHR surface = nullptr;

        VulkanSwapChain *swapChain = nullptr;
        VulkanCommandBuffer *commandBuffer = nullptr;
        VulkanDevice *vulkanDevice = nullptr;

        uint32 imageIndex = 0;
    };
}