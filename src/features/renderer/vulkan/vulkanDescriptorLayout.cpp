#include "features/renderer/vulkan/vulkanDescriptorLayout.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/logger/logger.h"
#include "vulkan/vulkan.h"

using namespace wne;

VulkanDescriptorLayout::VulkanDescriptorLayout(VulkanDevice *vulkanDevice)
{
    this->vulkanDevice = vulkanDevice;
}

VulkanDescriptorLayout::~VulkanDescriptorLayout()
{
    if (descriptorSetLayout)
        vkDestroyDescriptorSetLayout(vulkanDevice->getDevice(), descriptorSetLayout, nullptr);
}

bool VulkanDescriptorLayout::setup()
{
    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 0;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &samplerLayoutBinding;

    if (vkCreateDescriptorSetLayout(vulkanDevice->getDevice(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
    {
        Logger::log << "failed to create descriptor set layout!" << endl;
        return false;
    }

    return true;
}