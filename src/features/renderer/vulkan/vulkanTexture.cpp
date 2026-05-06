#include "features/renderer/vulkan/vulkanTexture.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <iostream>

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
    if (textureSampler)
        vkDestroySampler(device, textureSampler, nullptr);
}

bool VulkanTexture::setup(void *data, uint32 width, uint32 height)
{
    auto device = vulkanUtils->getVulkanDevice()->getDevice();
    auto physicalDevice = vulkanUtils->getVulkanDevice()->getPhysicalDevice();

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0; // Optional

    if (vkCreateImage(device, &imageInfo, nullptr, &textureImage) != VK_SUCCESS)
    {
        std::cout << "failed to create image" << std::endl;
        return false;
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, textureImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = vulkanUtils->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &textureImageMemory) != VK_SUCCESS)
    {
        std::cout << "failed to allocate image memory" << std::endl;
        return false;
    }

    vkBindImageMemory(device, textureImage, textureImageMemory, 0);

    vulkanUtils->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    vulkanUtils->copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height));

    vulkanUtils->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

    vulkanImageView = std::make_unique<VulkanImageView>(vulkanUtils->getVulkanDevice());
    if (!vulkanImageView->setup(textureImage, VK_FORMAT_R8G8B8A8_SRGB))
    {
        std::cout << "failed to create image view" << std::endl;
        return false;
    }

    // sampler
    // todo
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    if (vulkanUtils->isAnisotropySupported())
    {
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = std::min(4.0f, properties.limits.maxSamplerAnisotropy);
    }
    else
    {
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.maxAnisotropy = 1.0f;
    }

    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    // todo comparison might not be needed
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
    {
        std::cout << "failed to create texture sampler" << std::endl;
        return false;
    }

    return true;
}

std::shared_ptr<VulkanTexture> VulkanTexture::create(std::shared_ptr<Image> image, VulkanUtils *vulkanUtils)
{
    auto texture = std::make_shared<VulkanTexture>(vulkanUtils);
    if (texture->setup(image->getImageData(), image->getWidth(), image->getHeight()))
    {
        std::cout << "failed to make texture" << std::endl;
        return nullptr;
    }
    return texture;
}

void VulkanTexture::bind()
{
    auto device = vulkanUtils->getVulkanDevice()->getDevice();
    vkBindImageMemory(device, textureImage, textureImageMemory, 0);
}