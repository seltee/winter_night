#include "features/renderer/vulkan/vulkanImageView.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <iostream>

using namespace wne;

VulkanImageView::VulkanImageView(VulkanDevice *vulkanDevice)
{
    this->vulkanDevice = vulkanDevice;
}

VulkanImageView::~VulkanImageView()
{
    if (imageView)
        vkDestroyImageView(vulkanDevice->getDevice(), imageView, nullptr);
}

bool VulkanImageView::setup(VkImage image, uint64 format)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = (VkFormat)format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(vulkanDevice->getDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS)
    {
        std::cout << "failed to create image view" << std::endl;
        return false;
    }
    return true;
}