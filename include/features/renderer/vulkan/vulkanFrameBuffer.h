#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include <vector>

namespace wne
{
    class VulkanSwapChain;
    class VulkanRenderPass;
    class VulkanDepthPass;

    class VulkanFrameBuffer
    {
    public:
        VulkanFrameBuffer(VkDevice device);
        ~VulkanFrameBuffer();
        bool setup(VulkanSwapChain *swapChain, VulkanRenderPass *renderPass);
        bool setup(VulkanDepthPass *depthPass);

        inline const VkFramebuffer getFrameBuffer()
        {
            return frameBuffers.at(0);
        }

        inline const std::vector<VkFramebuffer> &getFrameBuffers()
        {
            return frameBuffers;
        }

    protected:
        std::vector<VkFramebuffer> frameBuffers;
        VkDevice device = nullptr;
    };
}