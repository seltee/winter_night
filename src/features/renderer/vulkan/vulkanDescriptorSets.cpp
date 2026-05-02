#include "features/renderer/vulkan/vulkanDescriptorSets.h"
#include "features/renderer/vulkan/vulkanDescriptorPool.h"
#include "features/renderer/vulkan/vulkanDescriptorLayout.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <iostream>

using namespace wne;

VulkanDescriptorSets::VulkanDescriptorSets(VulkanDevice *vulkanDevice)
{
    this->vulkanDevice = vulkanDevice;
}

VulkanDescriptorSets::~VulkanDescriptorSets()
{
}

bool VulkanDescriptorSets::setup(int maxFrames, VulkanDescriptorPool *vulkanDescriptorPool, VulkanDescriptorLayout *vulkanDescriptorLayout)
{
    std::vector<VkDescriptorSetLayout> layouts(maxFrames, *vulkanDescriptorLayout->getDescriptorLayout());
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = vulkanDescriptorPool->getDescriptorPool();
    allocInfo.descriptorSetCount = static_cast<uint32_t>(maxFrames);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(maxFrames);
    if (vkAllocateDescriptorSets(vulkanDevice->getDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
    {
        std::cout << "failed to allocate descriptor sets!" << std::endl;
        return false;
    }

    return true;
}