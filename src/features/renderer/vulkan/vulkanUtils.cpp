#include "features/renderer/vulkan/vulkanUtils.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#include "features/renderer/vulkan/vulkanCommandPool.h"
#include "features/renderer/vulkan/vulkanSampler.h"
#include "features/renderer/vulkan/vulkanDescriptorPool.h"
#include "features/renderer/vulkan/vulkanSwapChain.h"
#include "features/renderer/vulkan/vulkanRenderPass.h"
#include "features/renderer/vulkan/vulkanTexture.h"
#include "features/renderer/vulkan/pipelines/vulkanDescriptorSets.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <iostream>

using namespace wne;

VulkanUtils::VulkanUtils(
    VulkanDevice *vulkanDevice,
    VulkanCommandPool *vulkanCommandPool,
    VkQueue graphicsQueue,
    VkQueue presentQueue)
{
    this->vulkanDevice = vulkanDevice;
    this->device = vulkanDevice->getDevice();
    this->physicalDevice = vulkanDevice->getPhysicalDevice();
    this->vulkanCommandPool = vulkanCommandPool;
    this->graphicsQueue = graphicsQueue;
    this->presentQueue = presentQueue;

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(vulkanDevice->getPhysicalDevice(), &supportedFeatures);
    anisotropySupported = supportedFeatures.samplerAnisotropy;
}

VulkanUtils::~VulkanUtils()
{
}

bool VulkanUtils::setup()
{
    vulkanSampler = std::make_unique<VulkanSampler>(this);
    if (!vulkanSampler->setup(true))
    {
        std::cout << "Unable to create sampler" << std::endl;
        return false;
    }

    vulkanShadowSampler = std::make_unique<VulkanSampler>(this);
    if (!vulkanShadowSampler->setup(false))
    {
        std::cout << "Unable to create sampler" << std::endl;
        return false;
    }

    vulkanDescriptorPool = std::make_unique<VulkanDescriptorPool>(vulkanDevice);
    if (!vulkanDescriptorPool->setup())
    {
        std::cout << "Unable to create descriptor pool" << std::endl;
        return false;
    }

    vulkanObjectBuffers = std::make_unique<VulkanObjectBuffers>(this);
    if (!vulkanObjectBuffers->setup(MAX_FRAMES_IN_FLIGHT))
    {
        std::cout << "Unable to create objects buffer" << std::endl;
        return false;
    }

    vulkanPipelines = std::make_unique<VulkanPipelines>(vulkanDevice);
    vulkanShadowMaps = std::make_unique<VulkanShadowMaps>();

    uint8 pixels[16];
    memset(pixels, 0, 16);
    dummyTexture = std::make_unique<VulkanTexture>(this);
    dummyTexture->setup(pixels, 2, 2);

    return true;
}

int64 VulkanUtils::findMemoryType(uint32 typeFilter, uint64 properties) noexcept
{
    VkMemoryPropertyFlags flags = (VkMemoryPropertyFlags)properties;
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32 i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & flags) == flags)
        {
            return i;
        }
    }

    return -1;
}

VulkanFormat VulkanUtils::findSupportedFormat(const std::vector<VulkanFormat> &candidates, VulkanImageTiling tiling, VulkanFormatFeatureFlags features) noexcept
{
    for (auto format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, (VkFormat)format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }
    return VkFormat::VK_FORMAT_UNDEFINED;
}

uint VulkanUtils::getMSAAUsableSampleCount()
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT)
        return 64;
    if (counts & VK_SAMPLE_COUNT_32_BIT)
        return 32;
    if (counts & VK_SAMPLE_COUNT_16_BIT)
        return 16;
    if (counts & VK_SAMPLE_COUNT_8_BIT)
        return 8;
    if (counts & VK_SAMPLE_COUNT_4_BIT)
        return 4;
    if (counts & VK_SAMPLE_COUNT_2_BIT)
        return 2;

    return 1;
}

bool VulkanUtils::createBuffer(uint64 size, uint32 usage, uint32 properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
    {
        std::cout << "failed to create buffer" << std::endl;
        return false;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
    {
        std::cout << "failed to allocate buffer memory" << std::endl;
        return false;
    }

    vkBindBufferMemory(device, buffer, bufferMemory, 0);
    return true;
}

bool VulkanUtils::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, uint64 size)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    VkBufferMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_INDEX_READ_BIT;
    barrier.buffer = dstBuffer;
    barrier.offset = 0;
    barrier.size = size;

    vkCmdPipelineBarrier(commandBuffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
                         0,
                         0, nullptr,
                         1, &barrier,
                         0, nullptr);

    endSingleTimeCommands(commandBuffer);
    return true;
}

void VulkanUtils::transitionImageLayout(VkImage image, uint64 format, uint64 oldLayout, uint64 newLayout)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = (VkImageLayout)oldLayout;
    barrier.newLayout = (VkImageLayout)newLayout;

    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;
    barrier.subresourceRange.aspectMask =
        (format == VK_FORMAT_D32_SFLOAT || format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT)
            ? VK_IMAGE_ASPECT_DEPTH_BIT
            : VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier);

    endSingleTimeCommands(commandBuffer);
}

void VulkanUtils::copyBufferToImage(VkBuffer buffer, VkImage image, uint32 width, uint32 height)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        width,
        height,
        1};

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region);

    endSingleTimeCommands(commandBuffer);
}

void VulkanUtils::destroyPipelines()
{
    vulkanPipelines->reset();
}

bool VulkanUtils::rebuildPipelines(
    VulkanSwapChain *vulkanSwapChain,
    VulkanRenderPass *vulkanRenderPass,
    VulkanRenderPass *vulkanDepthPass,
    VulkanRenderPass *vulkanShadowDepthPass,
    uint sampleCount)
{
    this->vulkanRenderPass = vulkanRenderPass;
    this->vulkanDepthPass = vulkanDepthPass;
    this->vulkanShadowDepthPass = vulkanShadowDepthPass;

    VkDevice device = vulkanDevice->getDevice();
    vkDeviceWaitIdle(device);
    destroyPipelines();
    vkDeviceWaitIdle(device);

    if (!vulkanPipelines->build(
            vulkanSwapChain,
            vulkanRenderPass,
            vulkanDepthPass,
            vulkanShadowDepthPass,
            vulkanDescriptorPool.get(),
            vulkanObjectBuffers.get(),
            getVkSampleCountFlagBits(sampleCount)))
    {
        return false;
    }

    return true;
}

void VulkanUtils::updatePipelineShadowMaps()
{
    vulkanPipelines->updatePipelineShadowMaps(vulkanShadowMaps.get(), vulkanShadowSampler.get());
}

bool VulkanUtils::createImage(
    uint16 width,
    uint16 height,
    VulkanFormat format,
    uint64 numSamples,
    VulkanImageTiling tiling,
    VulkanImageUsageFlags usage,
    VulkanMemoryPropertyFlagBits memoryFlags,
    VkImage *pImage,
    VkDeviceMemory *pMemory)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = (VkFormat)format;
    imageInfo.tiling = (VkImageTiling)tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = (VkSampleCountFlagBits)getVkSampleCountFlagBits(numSamples);
    imageInfo.flags = 0; // Optional

    if (vkCreateImage(device, &imageInfo, nullptr, pImage) != VK_SUCCESS)
    {
        std::cout << "failed to create image" << std::endl;
        return false;
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, *pImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, memoryFlags);

    if (vkAllocateMemory(device, &allocInfo, nullptr, pMemory) != VK_SUCCESS)
    {
        vkDestroyImage(device, *pImage, nullptr);
        std::cout << "failed to allocate image memory" << std::endl;
        return false;
    }

    vkBindImageMemory(device, *pImage, *pMemory, 0);
    return true;
}

bool VulkanUtils::createImageView(VkImage image, VulkanFormat format, VulkanImageAspectFlags aspectFlags, VkImageView *imageView)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = (VkFormat)format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(vulkanDevice->getDevice(), &viewInfo, nullptr, imageView) != VK_SUCCESS)
    {
        std::cout << "failed to create image view" << std::endl;
        return false;
    }
    return true;
}

VkCommandBuffer VulkanUtils::beginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = vulkanCommandPool->getCommandPool();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void VulkanUtils::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, vulkanCommandPool->getCommandPool(), 1, &commandBuffer);
}

uint64 VulkanUtils::getVkSampleCountFlagBits(uint64 sampleCount)
{
    if (sampleCount == 64)
        return VK_SAMPLE_COUNT_64_BIT;
    if (sampleCount == 32)
        return VK_SAMPLE_COUNT_32_BIT;
    if (sampleCount == 16)
        return VK_SAMPLE_COUNT_16_BIT;
    if (sampleCount == 8)
        return VK_SAMPLE_COUNT_8_BIT;
    if (sampleCount == 4)
        return VK_SAMPLE_COUNT_4_BIT;
    if (sampleCount == 2)
        return VK_SAMPLE_COUNT_2_BIT;
    return VK_SAMPLE_COUNT_1_BIT;
}