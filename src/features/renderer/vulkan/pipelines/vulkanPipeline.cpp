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
    auto device = vulkanDevice->getDevice();
    if (graphicsPipeline)
    {
        vkDestroyPipeline(device, graphicsPipeline, nullptr);
        graphicsPipeline = nullptr;
    }
    if (pipelineLayout)
    {
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        pipelineLayout = nullptr;
    }
}

VkDescriptorSetLayout VulkanPipeline::getDescriptorSetLayout()
{
    return nullptr;
}