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
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/logger/logger.h"
#include "vulkan/vulkan.h"
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

bool VulkanFrameBuffer::setupColor(VulkanSwapChain *swapChain, VulkanRenderPass *renderPass, VulkanDepthBuffer *depthBuffer, uint64 sampleCount)
{
    auto swapChainImageViews = swapChain->getImageViews();
    auto swapChainSampledImageViews = swapChain->getSampledImageViews();
    VkExtent2D *swapChainExtent = swapChain->getExtent();

    frameBuffers.resize(swapChainImageViews->size());

    for (size_t i = 0; i < swapChainImageViews->size(); i++)
    {
        std::array<VkImageView, 3> attachments{};

        if (sampleCount == 1)
        {
            attachments[0] = swapChainImageViews->at(i)->getImageView();
            attachments[1] = depthBuffer->getDepthImageView();
        }
        else
        {
            attachments[0] = swapChainSampledImageViews->at(i)->getImageView();
            attachments[1] = depthBuffer->getDepthImageView();
            attachments[2] = swapChainImageViews->at(i)->getImageView();
        }

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass->getRenderPass();
        framebufferInfo.attachmentCount = (sampleCount == 1) ? 2 : 3;
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChainExtent->width;
        framebufferInfo.height = swapChainExtent->height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &frameBuffers[i]) != VK_SUCCESS)
        {
            Logger::log << "Failed creating vulkan frame buffer" << endl;
            return false;
        }
    }
    return true;
}

bool VulkanFrameBuffer::setupDepth(VulkanSwapChain *swapChain, VulkanRenderPass *depthPass, VulkanDepthBuffer *depthBuffer, uint64 sampleCount)
{
    auto swapChainImageViews = swapChain->getImageViews();
    VkExtent2D *swapChainExtent = swapChain->getExtent();

    frameBuffers.resize(swapChainImageViews->size());

    for (size_t i = 0; i < swapChainImageViews->size(); i++)
    {
        std::array<VkImageView, 1> attachments = {attachments[0] = depthBuffer->getDepthImageView()};

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
            Logger::log << "Failed creating vulkan frame buffer" << endl;
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
        Logger::log << "Failed creating vulkan frame buffer" << endl;
        return false;
    }

    return true;
}