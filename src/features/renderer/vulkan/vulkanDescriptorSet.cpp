#include "features/renderer/vulkan/vulkanDescriptorSet.h"
#include "features/renderer/vulkan/vulkanDescriptorPool.h"
#include "features/renderer/vulkan/vulkanDescriptorLayout.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <iostream>

using namespace wne;

VulkanDescriptorSet::VulkanDescriptorSet(VulkanDevice *vulkanDevice)
{
    this->vulkanDevice = vulkanDevice;
}

VulkanDescriptorSet::~VulkanDescriptorSet()
{
}

bool VulkanDescriptorSet::setup(VulkanDescriptorPool *vulkanDescriptorPool, VulkanDescriptorLayout *vulkanDescriptorLayout)
{
    VkDescriptorSetLayout layout = vulkanDescriptorLayout->getDescriptorLayout();
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = vulkanDescriptorPool->getDescriptorPool();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    if (vkAllocateDescriptorSets(vulkanDevice->getDevice(), &allocInfo, &descriptorSet) != VK_SUCCESS)
    {
        std::cout << "failed to allocate descriptor sets!" << std::endl;
        return false;
    }

    return true;
}