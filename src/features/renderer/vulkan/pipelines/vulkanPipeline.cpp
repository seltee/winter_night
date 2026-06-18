#include "features/renderer/vulkan/pipelines/vulkanPipeline.h"
#include "features/renderer/vulkan/vulkanShader.h"
#include "features/renderer/vulkan/vulkanRenderPass.h"
#include "features/renderer/vulkan/vulkanDescriptorLayout.h"
#include "features/renderer/vulkan/materials/vulkanMaterial.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include "core/data.h"
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

VulkanDescriptorSetLayout *VulkanPipeline::getDescriptorSetLayoutPipeline()
{
    return nullptr;
}

VulkanDescriptorSetLayout *VulkanPipeline::getDescriptorSetLayoutSampler()
{
    return nullptr;
}
