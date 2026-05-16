
#include "features/renderer/vulkan/vulkanPipelines.h"
#include "features/renderer/vulkan/vulkanShadowMaps.h"
#include "features/renderer/vulkan/vulkanDepthBuffer.h"
#include "features/renderer/vulkan/vulkanSampler.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <iostream>
#include <array>
#include <cmath>

using namespace wne;

VulkanPipelines::VulkanPipelines(VulkanDevice *vulkanDevice)
{
    this->vulkanDevice = vulkanDevice;
}

void VulkanPipelines::reset()
{
    if (vulkanPipelineColoredDepth)
        vulkanPipelineColoredDepth.reset();
    if (vulkanPipelineTexturedDepth)
        vulkanPipelineTexturedDepth.reset();
    if (vulkanPipelineColoredColor)
        vulkanPipelineColoredColor.reset();
    if (vulkanPipelineTexturedColor)
        vulkanPipelineTexturedColor.reset();
}

bool VulkanPipelines::build(
    VulkanSwapChain *vulkanSwapChain,
    VulkanRenderPass *vulkanRenderPass,
    VulkanRenderPass *VulkanDepthPass,
    VulkanDescriptorPool *vulkanDescriptorPool,
    VulkanObjectBuffers *vulkanObjectBuffers)
{
    reset();

    bool status = true;
    // depth pipelines
    vulkanPipelineColoredDepth = std::make_unique<VulkanPipelineColored>(vulkanDevice);
    status &= vulkanPipelineColoredDepth->setupDepth(vulkanRenderPass, vulkanDescriptorPool, vulkanObjectBuffers);

    vulkanPipelineTexturedDepth = std::make_unique<VulkanPipelineTextured>(vulkanDevice);
    status &= vulkanPipelineTexturedDepth->setupDepth(VulkanDepthPass);

    // color pipelines
    vulkanPipelineColoredColor = std::make_unique<VulkanPipelineColored>(vulkanDevice);
    status &= vulkanPipelineColoredColor->setupColor(vulkanRenderPass, vulkanDescriptorPool, vulkanObjectBuffers);

    vulkanPipelineTexturedColor = std::make_unique<VulkanPipelineTextured>(vulkanDevice);
    status &= vulkanPipelineTexturedColor->setupColor(vulkanRenderPass);

    vulkanDescriptorSets = std::make_unique<VulkanDescriptorSets>(vulkanDevice, vulkanDescriptorPool, vulkanObjectBuffers);
    if (!vulkanDescriptorSets->setup(2, vulkanPipelineTexturedDepth.get(), vulkanPipelineTexturedColor.get()))
    {
        std::cout << "Unable to create vulkan descriptor sets" << std::endl;
        return false;
    }

    if (!status)
    {
        std::cout << "Unable to create vulkan colored pipeline" << std::endl;
        return false;
    }
    return true;
}
void VulkanPipelines::enablePipelineColored(VulkanCommandBuffer *commandBuffer, bool isDepthRendering)
{
    if (isDepthRendering)
        currentPipeline = vulkanPipelineColoredDepth.get();
    else
        currentPipeline = vulkanPipelineColoredColor.get();
    commandBuffer->bindPipeline(currentPipeline);
}

void VulkanPipelines::enablePipelineTextured(VulkanCommandBuffer *commandBuffer, bool isDepthRendering)
{
    if (isDepthRendering)
    {
        currentPipeline = vulkanPipelineTexturedDepth.get();
    }
    else
    {
        currentPipeline = vulkanPipelineTexturedColor.get();
    }
    commandBuffer->bindPipeline(currentPipeline);
}
