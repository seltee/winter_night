#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanRenderPass.h"
#include "features/renderer/vulkan/vulkanSwapChain.h"
#include <vector>

namespace wne
{
    class VulkanFrameBuffer
    {
    public:
        VulkanFrameBuffer(VkDevice device);
        ~VulkanFrameBuffer();
        bool setup(VulkanSwapChain *swapChain, VulkanRenderPass *renderPass);

        inline const std::vector<VkFramebuffer> &getSwapChainFrameBuffers()
        {
            return swapChainFrameBuffers;
        }

    protected:
        std::vector<VkFramebuffer> swapChainFrameBuffers;
        VkDevice device = nullptr;
    };
}