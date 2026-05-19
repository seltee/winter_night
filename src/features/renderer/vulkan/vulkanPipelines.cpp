
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
    if (vulkanPipelineTexturedShadowDepth)
        vulkanPipelineTexturedShadowDepth.reset();
    if (vulkanPipelineTexturedMaskedShadowDepth)
        vulkanPipelineTexturedMaskedShadowDepth.reset();
    if (vulkanPipelineTexturedMaskedDepth)
        vulkanPipelineTexturedMaskedDepth.reset();
    if (vulkanPipelineColoredColor)
        vulkanPipelineColoredColor.reset();
    if (vulkanPipelineTexturedColor)
        vulkanPipelineTexturedColor.reset();
    if (vulkanPipelineAtmosphereColor)
        vulkanPipelineAtmosphereColor.reset();
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

    vulkanPipelineTexturedMaskedDepth = std::make_unique<VulkanPipelineTextured>(vulkanDevice);
    status &= vulkanPipelineTexturedMaskedDepth->setupMaskedDepth(VulkanDepthPass);

    vulkanPipelineTexturedShadowDepth = std::make_unique<VulkanPipelineTextured>(vulkanDevice);
    status &= vulkanPipelineTexturedShadowDepth->setupDepthShadow(VulkanDepthPass);

    vulkanPipelineTexturedMaskedShadowDepth = std::make_unique<VulkanPipelineTextured>(vulkanDevice);
    status &= vulkanPipelineTexturedMaskedShadowDepth->setupMaskedDepthShadow(VulkanDepthPass);

    // color pipelines
    vulkanPipelineColoredColor = std::make_unique<VulkanPipelineColored>(vulkanDevice);
    status &= vulkanPipelineColoredColor->setupColor(vulkanRenderPass, vulkanDescriptorPool, vulkanObjectBuffers);

    vulkanPipelineTexturedColor = std::make_unique<VulkanPipelineTextured>(vulkanDevice);
    status &= vulkanPipelineTexturedColor->setupColor(vulkanRenderPass);

    // atmosphere pipelines
    vulkanPipelineAtmosphereColor = std::make_unique<VulkanPipelineTextured>(vulkanDevice);
    status &= vulkanPipelineAtmosphereColor->setupAtmosphere(vulkanRenderPass);

    vulkanDescriptorSets = std::make_unique<VulkanDescriptorSets>(vulkanDevice, vulkanDescriptorPool, vulkanObjectBuffers);
    if (!vulkanDescriptorSets->setup(vulkanPipelineTexturedDepth.get(), vulkanPipelineTexturedColor.get()))
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

void VulkanPipelines::enablePipelineTextured(VulkanCommandBuffer *commandBuffer, bool isDepthRendering, bool isMasked)
{
    if (isDepthRendering)
    {
        if (isMasked)
            currentPipeline = vulkanPipelineTexturedMaskedDepth.get();
        else
            currentPipeline = vulkanPipelineTexturedDepth.get();
    }
    else
    {
        currentPipeline = vulkanPipelineTexturedColor.get();
    }
    commandBuffer->bindPipeline(currentPipeline);
}

void VulkanPipelines::enablePipelineTexturedShadowDepth(VulkanCommandBuffer *commandBuffer, bool isMasked)
{
    if (isMasked)
        currentPipeline = vulkanPipelineTexturedMaskedShadowDepth.get();
    else
        currentPipeline = vulkanPipelineTexturedShadowDepth.get();
    commandBuffer->bindPipeline(currentPipeline);
}

void VulkanPipelines::enablePipelineAtmosphere(VulkanCommandBuffer *commandBuffer)
{
    currentPipeline = vulkanPipelineAtmosphereColor.get();
    commandBuffer->bindPipeline(currentPipeline);
}