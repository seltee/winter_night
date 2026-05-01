/*
Commands in Vulkan, like drawing operations and memory transfers, are not executed directly using function calls.
You have to record all of the operations you want to perform in command buffer objects.
The advantage of this is that when we are ready to tell the Vulkan what we want to do,
all of the commands are submitted together and Vulkan can more efficiently process the commands since all of them are available together.
In addition, this allows command recording to happen in multiple threads if so desired.
*/

#include "features/renderer/vulkan/vulkanCommandBuffer.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <iostream>

using namespace wne;

VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice *vkDevice,
                                         VulkanRenderPass *renderPass,
                                         VulkanFrameBuffer *frameBuffer,
                                         VkExtent2D *swapChainExtent,
                                         VulkanCommandPool *commandPool)
{
    this->device = vkDevice->getDevice();
    this->physicalDevice = vkDevice->getPhysicalDevice();
    this->renderPass = renderPass;
    this->frameBuffer = frameBuffer;
    this->swapChainExtent = swapChainExtent;
    this->commandPool = commandPool;
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
}

bool VulkanCommandBuffer::setup(VkSurfaceKHR surface)
{
    // command buffer
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool->getCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS)
    {
        std::cout << "failed to allocate command buffers" << std::endl;
        return false;
    }

    return true;
}

void VulkanCommandBuffer::recordCommandBuffer(uint32_t imageIndex)
{
    // get rid of throw
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;                  // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        std::cout << "failed to begin recording command buffer" << std::endl;
        throw std::runtime_error("failed to begin recording command buffer");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass->getRenderPass();
    renderPassInfo.framebuffer = frameBuffer->getSwapChainFrameBuffers()[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = *swapChainExtent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandBuffer::bindPipeline(VulkanPipeline *vulkanPipeline)
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->getGraphicsPipeline());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChainExtent->width);
    viewport.height = static_cast<float>(swapChainExtent->height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = *swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void VulkanCommandBuffer::endRenderPass()
{
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
    vkCmdEndRenderPass(commandBuffer);
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        std::cout << "failed to record command buffer!" << std::endl;
        throw std::runtime_error("failed to record command buffer!");
    }
}

void VulkanCommandBuffer::resetBuffer()
{
    vkResetCommandBuffer(commandBuffer, 0);
}
