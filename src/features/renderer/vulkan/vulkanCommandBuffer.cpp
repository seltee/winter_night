/*
Commands in Vulkan, like drawing operations and memory transfers, are not executed directly using function calls.
You have to record all of the operations you want to perform in command buffer objects.
The advantage of this is that when we are ready to tell the Vulkan what we want to do,
all of the commands are submitted together and Vulkan can more efficiently process the commands since all of them are available together.
In addition, this allows command recording to happen in multiple threads if so desired.
*/

#include "features/renderer/vulkan/vulkanCommandBuffer.h"
#include "features/renderer/vulkan/vulkanQueueFamilies.h"
#include "features/renderer/vulkan/vulkanRenderPass.h"
#include "features/renderer/vulkan/vulkanFrameBuffer.h"
#include "features/renderer/vulkan/pipelines/vulkanPipeline.h"
#include "features/renderer/vulkan/vulkanCommandPool.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#include "features/logger/logger.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <array>

using namespace wne;

VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice *vulkanDevice,
                                         VulkanCommandPool *commandPool)
{
    this->device = vulkanDevice->getDevice();
    this->physicalDevice = vulkanDevice->getPhysicalDevice();
    this->commandPool = commandPool;
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
}

bool VulkanCommandBuffer::setup()
{
    // command buffer
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool->getCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS)
    {
        Logger::log << "failed to allocate command buffers" << endl;
        return false;
    }

    return true;
}

void VulkanCommandBuffer::recordCommandBuffer()
{
    // get rid of throw
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;                  // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        Logger::log << "failed to begin recording command buffer" << endl;
        throw std::runtime_error("failed to begin recording command buffer");
    }
}
// frameBuffer->getFrameBuffers()[imageIndex]
void VulkanCommandBuffer::beginRenderPass(VulkanRenderPass *renderPass, VkFramebuffer frameBuffer, uint16 width, uint16 height)
{
    passWidth = width;
    passHeight = height;

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass->getRenderPass();
    renderPassInfo.framebuffer = frameBuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = {passWidth, passHeight};

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil.depth = 1.0f;
    clearValues[1].depthStencil.stencil = 0;

    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandBuffer::beginDepthPass(VulkanRenderPass *depthPass, VkFramebuffer frameBuffer, uint16 width, uint16 height)
{
    passWidth = width;
    passHeight = height;

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = depthPass->getRenderPass();
    renderPassInfo.framebuffer = frameBuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = VkExtent2D{passWidth, passHeight};

    std::array<VkClearValue, 1> clearValues{};
    clearValues[0].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandBuffer::endPass()
{
    vkCmdEndRenderPass(commandBuffer);
}

void VulkanCommandBuffer::endCommandBuffer()
{
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        Logger::log << "failed to record command buffer!" << endl;
        throw std::runtime_error("failed to record command buffer!");
    }
}

void VulkanCommandBuffer::bindPipeline(VulkanPipeline *vulkanPipeline)
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->getGraphicsPipeline());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(passWidth);
    viewport.height = static_cast<float>(passHeight);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = {passWidth, passHeight};
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void VulkanCommandBuffer::resetBuffer()
{
    vkResetCommandBuffer(commandBuffer, 0);
}
