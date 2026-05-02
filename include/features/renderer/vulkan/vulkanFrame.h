#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include <vector>

namespace wne
{
    class VulkanDevice;
    class VulkanSwapChain;
    class VulkanRenderPass;
    class VulkanFrameBuffer;
    class VulkanCommandPool;
    class VulkanUtils;
    class VulkanPipeline;
    class VulkanCommandBuffer;

    class VulkanFrame
    {
    public:
        VulkanFrame(VulkanDevice *vulkanDevice, VulkanSwapChain *swapChain);
        ~VulkanFrame();
        bool setup(VkDescriptorSet vkDescriptorSet, VulkanRenderPass *renderPass, VulkanFrameBuffer *frameBuffer, VulkanCommandPool *commandPool, VulkanUtils *vulkanUtils);

        void startFrame(VulkanPipeline *pipeline);
        void finishFrame(VkQueue graphicsQueue, VkQueue presentQueue);

        void updateUniformBuffer(UniformBufferObject &uniformBufferObject);

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

        VkBuffer uniformBuffer = nullptr;
        VkDeviceMemory uniformBuffersMemory = nullptr;
        void *uniformBuffersMapped = nullptr;

        VulkanSwapChain *swapChain = nullptr;
        VulkanCommandBuffer *commandBuffer = nullptr;
        VulkanDevice *vulkanDevice = nullptr;

        uint32 imageIndex = 0;
    };
}