#include "features/renderer/vulkan/vulkanFrame.h"
#include "features/renderer/vulkan/vulkanCommandBuffer.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#include "features/renderer/vulkan/vulkanRenderPass.h"
#include "features/renderer/vulkan/vulkanFrameBuffer.h"
#include "features/renderer/vulkan/vulkanSwapChain.h"
#include "features/renderer/vulkan/vulkanUtils.h"
#include "features/renderer/vulkan/vulkanPipeline.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <iostream>

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
    if (uniformBuffer)
        vkDestroyBuffer(device, uniformBuffer, nullptr);
    if (uniformBuffersMemory)
        vkFreeMemory(device, uniformBuffersMemory, nullptr);
}

bool VulkanFrame::setup(VkDescriptorSet vkDescriptorSet, VulkanRenderPass *renderPass, VulkanFrameBuffer *frameBuffer, VulkanCommandPool *commandPool, VulkanUtils *vulkanUtils)
{
    commandBuffer = new VulkanCommandBuffer(vulkanDevice, renderPass, frameBuffer, swapChain->getExtent(), commandPool);
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

    // create uniform buffer to pass parameters of the shader
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    vulkanUtils->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffer, uniformBuffersMemory);
    vkMapMemory(device, uniformBuffersMemory, 0, bufferSize, 0, &uniformBuffersMapped);

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = uniformBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformBufferObject);

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = vkDescriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;
    descriptorWrite.pImageInfo = nullptr;       // Optional
    descriptorWrite.pTexelBufferView = nullptr; // Optional

    vkUpdateDescriptorSets(vulkanDevice->getDevice(), 1, &descriptorWrite, 0, nullptr);

    return true;
}

void VulkanFrame::startFrame(VulkanPipeline *pipeline)
{
    vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &inFlightFence);

    vkAcquireNextImageKHR(device, swapChain->getSwapChain(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    commandBuffer->resetBuffer();
    commandBuffer->recordCommandBuffer(imageIndex);
    commandBuffer->bindPipeline(pipeline);
}

void VulkanFrame::finishFrame(VkQueue graphicsQueue, VkQueue presentQueue)
{
    // get rid of throw
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

void VulkanFrame::updateUniformBuffer(UniformBufferObject &uniformBufferObject)
{
    memcpy(uniformBuffersMapped, &uniformBufferObject, sizeof(UniformBufferObject));
}