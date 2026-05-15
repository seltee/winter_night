#include "features/renderer/vulkan/vulkanFrame.h"
#include "features/renderer/vulkan/vulkanCommandBuffer.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#include "features/renderer/vulkan/vulkanRenderPass.h"
#include "features/renderer/vulkan/vulkanFrameBuffer.h"
#include "features/renderer/vulkan/vulkanSwapChain.h"
#include "features/renderer/vulkan/vulkanUtils.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <iostream>
#include <array>

using namespace wne;

VulkanFrame::VulkanFrame(VulkanDevice *vulkanDevice, VulkanSwapChain *swapChain)
{
    this->vulkanDevice = vulkanDevice;
    this->device = vulkanDevice->getDevice();
    this->physicalDevice = vulkanDevice->getPhysicalDevice();
    this->surface = vulkanDevice->getSurface();
    this->swapChain = swapChain;
}

VulkanFrame::~VulkanFrame()
{
    if (imageAvailableSemaphore)
        vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
    if (renderFinishedSemaphore)
        vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
    if (inFlightFence)
        vkDestroyFence(device, inFlightFence, nullptr);
}

bool VulkanFrame::setup(
    VulkanCommandPool *commandPool,
    VulkanUtils *vulkanUtils)
{
    commandBuffer = new VulkanCommandBuffer(vulkanDevice, commandPool);
    if (!commandBuffer->setup(surface))
    {
        std::cout << "Unable to create vulkan command buffer" << std::endl;
        return false;
    }

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS)
    {
        std::cout << "failed to create semaphores!" << std::endl;
        return false;
    }

    return true;
}

void VulkanFrame::startFrame()
{
    vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &inFlightFence);

    vkAcquireNextImageKHR(device, swapChain->getSwapChain(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    commandBuffer->resetBuffer();
    commandBuffer->recordCommandBuffer();
}

void VulkanFrame::beginDepthPass(VulkanRenderPass *depthPass, VulkanFrameBuffer *frameBuffers)
{
    auto extent = swapChain->getExtent();
    commandBuffer->beginDepthPass(depthPass, frameBuffers->getFrameBuffers()[imageIndex], extent->width, extent->height);
}

void VulkanFrame::endDepthPass()
{
    commandBuffer->endPass();
}

void VulkanFrame::beginRenderPass(VulkanRenderPass *renderPass, VulkanFrameBuffer *frameBuffers)
{
    auto extent = swapChain->getExtent();
    commandBuffer->beginRenderPass(renderPass, frameBuffers->getFrameBuffers()[imageIndex], extent->width, extent->height);
}

void VulkanFrame::finishFrame(VkQueue graphicsQueue, VkQueue presentQueue)
{
    // get rid of throw
    commandBuffer->endPass();
    commandBuffer->endCommandBuffer();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    VkCommandBuffer commandBufferNative = commandBuffer->getCommandBuffer();
    submitInfo.pCommandBuffers = &commandBufferNative;

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS)
    {
        // todo handle this error
        std::cout << "failed to submit draw command buffer!" << std::endl;
        return;
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain->getSwapChain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    vkQueuePresentKHR(presentQueue, &presentInfo);
}
