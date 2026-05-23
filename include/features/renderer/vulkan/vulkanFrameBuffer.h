#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include <vector>

namespace wne
{
    class VulkanSwapChain;
    class VulkanRenderPass;
    class VulkanDepthBuffer;

    class VulkanFrameBuffer
    {
    public:
        VulkanFrameBuffer(VkDevice device);
        ~VulkanFrameBuffer();
        bool setupColor(VulkanSwapChain *swapChain, VulkanRenderPass *renderPass, VulkanDepthBuffer *depthBuffer, uint64 sampleCount);
        bool setupDepth(VulkanSwapChain *swapChain, VulkanRenderPass *depthPass, VulkanDepthBuffer *depthBuffer, uint64 sampleCount);
        bool setup(VulkanRenderPass *depthPass, VulkanDepthBuffer *depthBuffer);

        inline const VkFramebuffer getFrameBuffer()
        {
            return frameBuffers.at(0);
        }

        inline const std::vector<VkFramebuffer> &getFrameBuffers()
        {
            return frameBuffers;
        }

        inline VulkanRenderPass *getDepthPass()
        {
            return depthPass;
        }

    protected:
        VulkanRenderPass *depthPass = nullptr;
        std::vector<VkFramebuffer> frameBuffers;
        VkDevice device = nullptr;
    };
}