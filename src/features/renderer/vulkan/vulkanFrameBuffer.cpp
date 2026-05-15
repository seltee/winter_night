/* ===
The attachments specified during render pass creation are bound by wrapping them into a VkFramebuffer object.
A framebuffer object references all of the VkImageView objects that represent the attachments.
In our case that will be only a single one: the color attachment.
However, the image that we have to use for the attachment depends on which image the swap chain returns when we retrieve one for presentation.
That means that we have to create a framebuffer for all of the images in the swap chain and use the one that corresponds to the retrieved image at drawing time.
=== */

#include "features/renderer/vulkan/vulkanFrameBuffer.h"
#include "features/renderer/vulkan/vulkanRenderPass.h"
#include "features/renderer/vulkan/vulkanSwapChain.h"
#include "features/renderer/vulkan/vulkanDepthBuffer.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <iostream>
#include <array>

using namespace wne;

VulkanFrameBuffer::VulkanFrameBuffer(VkDevice device)
{
    this->device = device;
}

VulkanFrameBuffer::~VulkanFrameBuffer()
{
    for (auto framebuffer : frameBuffers)
    {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }
}

bool VulkanFrameBuffer::setupColor(VulkanSwapChain *swapChain, VulkanRenderPass *renderPass, VulkanDepthBuffer *depthBuffer)
{
    auto swapChainImageViews = swapChain->getImageViews();
    VkExtent2D *swapChainExtent = swapChain->getExtent();

    frameBuffers.resize(swapChainImageViews->size());

    for (size_t i = 0; i < swapChainImageViews->size(); i++)
    {
        std::array<VkImageView, 2> attachments = {
            swapChainImageViews->at(i)->getImageView(),
            depthBuffer->getDepthImageView()};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass->getRenderPass();
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChainExtent->width;
        framebufferInfo.height = swapChainExtent->height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &frameBuffers[i]) != VK_SUCCESS)
        {
            std::cout << "Failed creating vulkan frame buffer" << std::endl;
            return false;
        }
    }
    return true;
}

bool VulkanFrameBuffer::setupDepth(VulkanSwapChain *swapChain, VulkanRenderPass *depthPass, VulkanDepthBuffer *depthBuffer)
{
    auto swapChainImageViews = swapChain->getImageViews();
    VkExtent2D *swapChainExtent = swapChain->getExtent();

    frameBuffers.resize(swapChainImageViews->size());

    for (size_t i = 0; i < swapChainImageViews->size(); i++)
    {
        std::array<VkImageView, 1> attachments = {depthBuffer->getDepthImageView()};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = depthPass->getRenderPass();
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChainExtent->width;
        framebufferInfo.height = swapChainExtent->height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &frameBuffers[i]) != VK_SUCCESS)
        {
            std::cout << "Failed creating vulkan frame buffer" << std::endl;
            return false;
        }
    }
    return true;
}

bool VulkanFrameBuffer::setup(VulkanRenderPass *depthPass, VulkanDepthBuffer *depthBuffer)
{
    this->depthPass = depthPass;
    std::array<VkImageView, 1> attachments = {depthBuffer->getDepthImageView()};

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = depthPass->getRenderPass();
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = depthBuffer->getWidth();
    framebufferInfo.height = depthBuffer->getHeight();
    framebufferInfo.layers = 1;

    frameBuffers.resize(1);
    if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &frameBuffers[0]) != VK_SUCCESS)
    {
        std::cout << "Failed creating vulkan frame buffer" << std::endl;
        return false;
    }

    return true;
}