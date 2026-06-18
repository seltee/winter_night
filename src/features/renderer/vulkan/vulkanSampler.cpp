#include "features/renderer/vulkan/vulkanSampler.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/logger/logger.h"
#include "vulkan/vulkan.h"
#include <array>

using namespace wne;

VulkanSampler::VulkanSampler(VulkanUtils *vulkanUtils)
{
    this->vulkanUtils = vulkanUtils;
}

VulkanSampler::~VulkanSampler()
{
    auto device = vulkanUtils->getVulkanDevice()->getDevice();
    if (textureSampler)
        vkDestroySampler(device, textureSampler, nullptr);
}

bool VulkanSampler::setup(bool UVRepeat)
{
    auto device = vulkanUtils->getVulkanDevice()->getDevice();
    auto physicalDevice = vulkanUtils->getVulkanDevice()->getPhysicalDevice();

    // sampler
    // todo
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

    VkSamplerAddressMode addressMode = UVRepeat ? VK_SAMPLER_ADDRESS_MODE_REPEAT : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = addressMode;
    samplerInfo.addressModeV = addressMode;
    samplerInfo.addressModeW = addressMode;
    samplerInfo.borderColor = UVRepeat ? VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK : VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

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
        Logger::log << "failed to create texture sampler" << endl;
        return false;
    }

    return true;
}