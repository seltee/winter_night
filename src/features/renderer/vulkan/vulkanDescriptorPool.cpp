/*
    Descriptor sets can't be created directly, they must be allocated from a pool like command buffers.
    The equivalent for descriptor sets is unsurprisingly called a descriptor pool.
*/

#include "features/renderer/vulkan/vulkanDescriptorPool.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <iostream>
#include <array>

using namespace wne;

VulkanDescriptorPool::VulkanDescriptorPool(VulkanDevice *vulkanDevice)
{
    this->vulkanDevice = vulkanDevice;
}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
    if (descriptorPool)
        vkDestroyDescriptorPool(vulkanDevice->getDevice(), descriptorPool, nullptr);
}

bool VulkanDescriptorPool::setup(int maxFrames)
{
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(maxFrames);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(maxFrames);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(maxFrames);

    if (vkCreateDescriptorPool(vulkanDevice->getDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
    {
        std::cout << "failed to create descriptor pool!" << std::endl;
        return false;
    }

    return true;
}