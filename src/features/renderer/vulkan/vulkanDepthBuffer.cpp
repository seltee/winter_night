#include "features/renderer/vulkan/vulkanDepthBuffer.h"
#include "features/renderer/vulkan/vulkanUtils.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"

using namespace wne;

VulkanDepthBuffer::VulkanDepthBuffer(VulkanUtils *vulkanUtils)
{
    this->vulkanUtils = vulkanUtils;
}

VulkanDepthBuffer::~VulkanDepthBuffer()
{
}

bool VulkanDepthBuffer::setup(uint16 width, uint16 height, bool isSampled)
{
    this->width = width;
    this->height = height;
    format = findDepthFormat(isSampled);

    if (!vulkanUtils->createImage(
            width,
            height,
            format,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | (isSampled ? VK_IMAGE_USAGE_SAMPLED_BIT : 0),
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            &depthImage, &depthImageMemory))
    {
        std::cout << "Failed to create depth buffer image" << std::endl;
        return false;
    }

    vulkanUtils->createImageView(depthImage, format, VK_IMAGE_ASPECT_DEPTH_BIT, &depthImageView);
    return true;
}

VulkanFormat VulkanDepthBuffer::findDepthFormat(bool isSampled)
{
    return vulkanUtils->findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | (isSampled ? VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT : 0));
}

bool VulkanDepthBuffer::hasStencilComponent(VulkanFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}