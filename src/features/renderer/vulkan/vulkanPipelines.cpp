#include "features/renderer/vulkan/vulkanPipelines.h"
#include <iostream>

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
    VulkanDepthPass *VulkanDepthPass,
    VulkanDescriptorPool *vulkanDescriptorPool,
    VulkanObjectBuffers *vulkanObjectBuffers)
{
    reset();

    bool status = true;
    // depth pipelines
    vulkanPipelineColoredDepth = std::make_unique<VulkanPipelineColored>(vulkanDevice);
    status &= vulkanPipelineColoredDepth->setup(vulkanRenderPass, vulkanDescriptorPool, vulkanObjectBuffers, true);

    vulkanPipelineTexturedDepth = std::make_unique<VulkanPipelineTextured>(vulkanDevice);
    status &= vulkanPipelineTexturedDepth->setup(vulkanRenderPass, vulkanDescriptorPool, vulkanObjectBuffers, true);

    // color pipelines
    vulkanPipelineColoredColor = std::make_unique<VulkanPipelineColored>(vulkanDevice);
    status &= vulkanPipelineColoredColor->setup(vulkanRenderPass, vulkanDescriptorPool, vulkanObjectBuffers, false);

    vulkanPipelineTexturedColor = std::make_unique<VulkanPipelineTextured>(vulkanDevice);
    status &= vulkanPipelineTexturedColor->setup(vulkanRenderPass, vulkanDescriptorPool, vulkanObjectBuffers, false);

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
        currentPipeline = vulkanPipelineTexturedDepth.get();
    else
        currentPipeline = vulkanPipelineTexturedColor.get();
    commandBuffer->bindPipeline(currentPipeline);
}
