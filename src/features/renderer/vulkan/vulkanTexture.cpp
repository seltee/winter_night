#include "features/renderer/vulkan/vulkanTexture.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/logger/logger.h"
#include "vulkan/vulkan.h"

using namespace wne;

VulkanTexture::VulkanTexture(VulkanUtils *vulkanUtils)
{
    this->vulkanUtils = vulkanUtils;
}

VulkanTexture::~VulkanTexture()
{
    auto device = vulkanUtils->getVulkanDevice()->getDevice();
    if (textureImage)
        vkDestroyImage(device, textureImage, nullptr);
    if (textureImageMemory)
        vkFreeMemory(device, textureImageMemory, nullptr);
}

bool VulkanTexture::setup(void *pixels, uint32 width, uint32 height)
{
    auto device = vulkanUtils->getVulkanDevice()->getDevice();
    if (textureImage)
    {
        vkDestroyImage(device, textureImage, nullptr);
        textureImage = nullptr;
    }
    if (textureImageMemory)
    {
        vkFreeMemory(device, textureImageMemory, nullptr);
        textureImageMemory = nullptr;
    }

    uint64 imageSize = width * height * 4;
    if (!vulkanUtils->createBuffer(
            imageSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory))
    {
        Logger::log << "failed to create texture buffer" << endl;
        return false;
    }
    void *bufferData;
    vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &bufferData);
    memcpy(bufferData, pixels, static_cast<size_t>(imageSize));

    if (!vulkanUtils->createImage(
            width,
            height,
            VK_FORMAT_R8G8B8A8_SRGB,
            1,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            &textureImage,
            &textureImageMemory))
    {
        Logger::log << "failed to create image" << endl;
        return false;
    }

    vulkanUtils->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    vulkanUtils->copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height));

    vulkanUtils->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkUnmapMemory(device, stagingBufferMemory);
    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

    vulkanImageView = std::make_unique<VulkanImageView>(vulkanUtils->getVulkanDevice());
    if (!vulkanImageView->setup(textureImage, VK_FORMAT_R8G8B8A8_SRGB))
    {
        Logger::log << "failed to create image view" << endl;
        return false;
    }
    imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    return true;
}

bool VulkanTexture::setup(VkImageView vkImageView)
{
    vulkanImageView = std::make_unique<VulkanImageView>(vulkanUtils->getVulkanDevice());
    if (!vulkanImageView->setup(vkImageView))
    {
        Logger::log << "failed to create image view" << endl;
        return false;
    }
    imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    return true;
}

std::shared_ptr<VulkanTexture> VulkanTexture::create(std::shared_ptr<Image> image, VulkanUtils *vulkanUtils)
{
    auto texture = std::make_shared<VulkanTexture>(vulkanUtils);
    if (!texture->setup(image->getImageData(), image->getWidth(), image->getHeight()))
    {
        Logger::log << "failed to make texture" << endl;
        return nullptr;
    }
    return texture;
}

void VulkanTexture::bind()
{
    auto device = vulkanUtils->getVulkanDevice()->getDevice();
    vkBindImageMemory(device, textureImage, textureImageMemory, 0);
}