
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
    if (vulkanPipelineAtmosphereColor)
        vulkanPipelineAtmosphereColor.reset();

    for (uint8 lightState = 0; lightState < val(LightState::Total); lightState++)
    {
        for (uint8 colorState = 0; colorState < val(ColorBlending::Total); colorState++)
        {
            vulkanPipelineTextured[lightState][colorState].reset();
        }
    }
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

    for (uint8 lightState = 0; lightState < val(LightState::Total); lightState++)
    {
        for (uint8 blending = 0; blending < val(ColorBlending::Total); blending++)
        {
            vulkanPipelineTextured[lightState][blending] = std::make_unique<VulkanPipelineTextured>(vulkanDevice);
            status &= vulkanPipelineTextured[lightState][blending]->setupParametred(vulkanRenderPass, lightState, (ColorBlending)blending);
        }
    }

    // atmosphere pipelines
    vulkanPipelineAtmosphereColor = std::make_unique<VulkanPipelineTextured>(vulkanDevice);
    status &= vulkanPipelineAtmosphereColor->setupAtmosphere(vulkanRenderPass);

    vulkanDescriptorSets = std::make_unique<VulkanDescriptorSets>(vulkanDevice, vulkanDescriptorPool, vulkanObjectBuffers);
    if (!vulkanDescriptorSets->setup(vulkanPipelineTexturedDepth.get(), vulkanPipelineTextured[val(LightState::Disabled)][val(ColorBlending::Solid)].get()))
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

void VulkanPipelines::enablePipelineTexturedDepth(VulkanCommandBuffer *commandBuffer, bool isMasked)
{
    if (isMasked)
        currentPipeline = vulkanPipelineTexturedMaskedDepth.get();
    else
        currentPipeline = vulkanPipelineTexturedDepth.get();
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

void VulkanPipelines::enablePipelineTextured(VulkanCommandBuffer *commandBuffer, bool isLightEnabled, ColorBlending blending)
{
    currentPipeline = vulkanPipelineTextured[isLightEnabled ? val(LightState::Enabled) : val(LightState::Disabled)][val(blending)].get();
    commandBuffer->bindPipeline(currentPipeline);
}

void VulkanPipelines::enablePipelineAtmosphere(VulkanCommandBuffer *commandBuffer)
{
    currentPipeline = vulkanPipelineAtmosphereColor.get();
    commandBuffer->bindPipeline(currentPipeline);
}