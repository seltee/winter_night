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
        VulkanFrame(VulkanDevice *vDevice, VulkanSwapChain *swapChain);
        ~VulkanFrame();
        bool setup(VulkanRenderPass *renderPass, VulkanFrameBuffer *frameBuffer);

        void startFrame(VulkanPipeline *pipeline);
        void finishFrame(VkQueue graphicsQueue, VkQueue presentQueue);

        inline VulkanCommandBuffer *getCommandBuffer()
        {
            return commandBuffer;
        }

    protected:
        VulkanSwapChain *swapChain = nullptr;
        VulkanCommandBuffer *commandBuffer = nullptr;
        VkSemaphore imageAvailableSemaphore = nullptr;
        VkSemaphore renderFinishedSemaphore = nullptr;
        VkFence inFlightFence = nullptr;
        VkDevice device;
        VkPhysicalDevice physicalDevice;
        VkSurfaceKHR surface;

        uint32 imageIndex = 0;
    };
}