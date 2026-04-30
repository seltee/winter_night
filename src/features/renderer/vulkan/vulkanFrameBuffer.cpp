/* ===
The attachments specified during render pass creation are bound by wrapping them into a VkFramebuffer object.
A framebuffer object references all of the VkImageView objects that represent the attachments.
In our case that will be only a single one: the color attachment.
However, the image that we have to use for the attachment depends on which image the swap chain returns when we retrieve one for presentation.
That means that we have to create a framebuffer for all of the images in the swap chain and use the one that corresponds to the retrieved image at drawing time.
=== */

#include "features/renderer/vulkan/vulkanFrameBuffer.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <iostream>

using namespace WNE;

VulkanFrameBuffer::VulkanFrameBuffer(VkDevice device)
{
    this->device = device;
}

VulkanFrameBuffer::~VulkanFrameBuffer()
{
    for (auto framebuffer : swapChainFrameBuffers)
    {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }
}

bool VulkanFrameBuffer::setup(VulkanSwapChain *swapChain, VulkanRenderPass *renderPass)
{
    std::vector<VkImageView> *swapChainImageViews = swapChain->getImageViews();
    VkExtent2D *swapChainExtent = swapChain->getExtent();

    swapChainFrameBuffers.resize(swapChainImageViews->size());

    for (size_t i = 0; i < swapChainImageViews->size(); i++)
    {
        VkImageView attachments[] = {
            (*swapChainImageViews)[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass->getRenderPass();
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapChainExtent->width;
        framebufferInfo.height = swapChainExtent->height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFrameBuffers[i]) != VK_SUCCESS)
        {
            std::cout << "Failed creating vulkan frame buffer" << std::endl;
            return false;
        }
    }
    return true;
}