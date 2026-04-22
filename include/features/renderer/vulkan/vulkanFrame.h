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
        void render(VulkanPipeline *pipeline, VkQueue graphicsQueue, VkQueue presentQueue);

    protected:
        VulkanSwapChain *swapChain = nullptr;
        VulkanCommandBuffer *commandBuffer = nullptr;
        VkSemaphore imageAvailableSemaphore = nullptr;
        VkSemaphore renderFinishedSemaphore = nullptr;
        VkFence inFlightFence = nullptr;
        VkDevice device;
        VkPhysicalDevice physicalDevice;
        VkSurfaceKHR surface;
    };
}