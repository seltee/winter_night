#include "features/renderer/vulkan/vulkanDepthBuffer.h"
#include "features/renderer/vulkan/vulkanUtils.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/logger/logger.h"
#include "vulkan/vulkan.h"

using namespace wne;

VulkanDepthBuffer::VulkanDepthBuffer(VulkanUtils *vulkanUtils)
{
    this->vulkanUtils = vulkanUtils;
}

VulkanDepthBuffer::~VulkanDepthBuffer()
{
    auto device = vulkanUtils->getVulkanDevice()->getDevice();
    if (depthImageView)
        vkDestroyImageView(device, depthImageView, nullptr);
    if (depthImage)
        vkDestroyImage(device, depthImage, nullptr);
}

bool VulkanDepthBuffer::setup(int width, int height, uint64 sampledCountBit, bool isSampled)
{
    this->width = width;
    this->height = height;
    format = vulkanUtils->findDepthFormat(isSampled);

    if (!vulkanUtils->createImage(
            width,
            height,
            format,
            sampledCountBit,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | (isSampled ? VK_IMAGE_USAGE_SAMPLED_BIT : 0),
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            &depthImage, &depthImageMemory))
    {
        Logger::log << "Failed to create depth buffer image" << endl;
        return false;
    }

    vulkanUtils->createImageView(depthImage, format, VK_IMAGE_ASPECT_DEPTH_BIT, &depthImageView);
    return true;
}

void VulkanDepthBuffer::transitionToDefined()
{
    vulkanUtils->transitionImageLayout(depthImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
}

bool VulkanDepthBuffer::hasStencilComponent(VulkanFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}