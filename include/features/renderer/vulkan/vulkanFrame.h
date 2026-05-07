#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanSampler.h"
#include <vector>

namespace wne
{
    class VulkanDevice;
    class VulkanSwapChain;
    class VulkanRenderPass;
    class VulkanFrameBuffer;
    class VulkanCommandPool;
    class VulkanUtils;
    class VulkanCommandBuffer;

    class VulkanFrame
    {
    public:
        VulkanFrame(VulkanDevice *vulkanDevice, VulkanSwapChain *swapChain);
        ~VulkanFrame();
        bool setup(
            VulkanRenderPass *renderPass,
            VulkanFrameBuffer *frameBuffer,
            VulkanCommandPool *commandPool,
            VulkanUtils *vulkanUtils);

        void startFrame();
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