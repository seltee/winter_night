#include "features/renderer/vulkan/vulkanFrame.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"

VulkanFrame::VulkanFrame(VulkanDevice *vDevice, VulkanSwapChain *swapChain)
{
    this->device = vDevice->getDevice();
    this->physicalDevice = vDevice->getPhysicalDevice();
    this->surface = vDevice->getSurface();
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

bool VulkanFrame::setup(VulkanRenderPass *renderPass, VulkanFrameBuffer *frameBuffer)
{
    commandBuffer = new VulkanCommandBuffer(device, physicalDevice, renderPass, frameBuffer, swapChain->getExtent());
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

void VulkanFrame::render(VulkanPipeline *pipeline, VkQueue graphicsQueue, VkQueue presentQueue)
{
    vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &inFlightFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(device, swapChain->getSwapChain(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    commandBuffer->resetBuffer();
    commandBuffer->recordCommandBuffer(imageIndex);
    commandBuffer->bindPipeline(pipeline);
    commandBuffer->endRenderPass();

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
        throw std::runtime_error("failed to submit draw command buffer!");
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