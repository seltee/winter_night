#include "features/renderer/vulkan/pipelines/vulkanPipeline.h"
#include "features/renderer/vulkan/vulkanShader.h"
#include "features/renderer/vulkan/vulkanRenderPass.h"
#include "features/renderer/vulkan/vulkanDescriptorLayout.h"
#include "features/renderer/vulkan/materials/vulkanMaterial.h"
#include "core/data.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <vector>
#include <iostream>

using namespace wne;

VulkanPipeline::VulkanPipeline(VulkanDevice *vulkanDevice)
{
    this->vulkanDevice = vulkanDevice;
}

VulkanPipeline::~VulkanPipeline()
{
}

VkPipeline VulkanPipeline::getGraphicsPipeline()
{
    return nullptr;
}

VkPipelineLayout VulkanPipeline::getPipelineLayout()
{
    return nullptr;
}

VkDescriptorSetLayout VulkanPipeline::getDescriptorSetLayoutPipeline()
{
    return nullptr;
}

VkDescriptorSetLayout VulkanPipeline::getDescriptorSetLayoutSampler()
{
    return nullptr;
}

VkDescriptorSet VulkanPipeline::getDescriptorSet()
{
    return nullptr;
}