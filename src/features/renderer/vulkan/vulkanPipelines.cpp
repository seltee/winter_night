
#include "features/renderer/vulkan/vulkanPipelines.h"
#include "features/renderer/vulkan/vulkanShadowMaps.h"
#include "features/renderer/vulkan/vulkanDepthBuffer.h"
#include "features/renderer/vulkan/vulkanSampler.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/logger/logger.h"
#include "vulkan/vulkan.h"
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
    if (vulkanPipelineTexturedSingleSideMaskedShadowDepth)
        vulkanPipelineTexturedSingleSideMaskedShadowDepth.reset();
    if (vulkanPipelineTexturedSingleSideShadowDepth)
        vulkanPipelineTexturedSingleSideShadowDepth.reset();
    if (vulkanPipelineTexturedDoubleSideMaskedShadowDepth)
        vulkanPipelineTexturedDoubleSideMaskedShadowDepth.reset();
    if (vulkanPipelineTexturedDoubleSideShadowDepth)
        vulkanPipelineTexturedDoubleSideShadowDepth.reset();
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
    VulkanRenderPass *VulkanShadowDepthPass,
    VulkanDescriptorPool *vulkanDescriptorPool,
    VulkanObjectBuffers *vulkanObjectBuffers,
    uint64 MSAASampleCountBit)
{
    reset();

    bool status = true;
    // depth pipelines
    // vulkanPipelineColoredDepth = std::make_unique<VulkanPipelineColored>(vulkanDevice);
    // status &= vulkanPipelineColoredDepth->setupDepth(vulkanRenderPass, vulkanDescriptorPool, vulkanObjectBuffers);

    vulkanPipelineTexturedDepth = std::make_unique<VulkanPipelineTextured>(vulkanDevice);
    status &= vulkanPipelineTexturedDepth->setupDepth(VulkanDepthPass, MSAASampleCountBit);

    vulkanPipelineTexturedMaskedDepth = std::make_unique<VulkanPipelineTextured>(vulkanDevice);
    status &= vulkanPipelineTexturedMaskedDepth->setupMaskedDepth(VulkanDepthPass, MSAASampleCountBit);

    vulkanPipelineTexturedSingleSideShadowDepth = std::make_unique<VulkanPipelineTextured>(vulkanDevice);
    status &= vulkanPipelineTexturedSingleSideShadowDepth->setupDepthShadow(VulkanShadowDepthPass, false);

    vulkanPipelineTexturedSingleSideMaskedShadowDepth = std::make_unique<VulkanPipelineTextured>(vulkanDevice);
    status &= vulkanPipelineTexturedSingleSideMaskedShadowDepth->setupMaskedDepthShadow(VulkanShadowDepthPass, false);

    vulkanPipelineTexturedDoubleSideShadowDepth = std::make_unique<VulkanPipelineTextured>(vulkanDevice);
    status &= vulkanPipelineTexturedDoubleSideShadowDepth->setupDepthShadow(VulkanShadowDepthPass, true);

    vulkanPipelineTexturedDoubleSideMaskedShadowDepth = std::make_unique<VulkanPipelineTextured>(vulkanDevice);
    status &= vulkanPipelineTexturedDoubleSideMaskedShadowDepth->setupMaskedDepthShadow(VulkanShadowDepthPass, true);

    // color pipelines
    // vulkanPipelineColoredColor = std::make_unique<VulkanPipelineColored>(vulkanDevice);
    // status &= vulkanPipelineColoredColor->setupColor(vulkanRenderPass, vulkanDescriptorPool, vulkanObjectBuffers);

    for (uint8 lightState = 0; lightState < val(LightState::Total); lightState++)
    {
        for (uint8 blending = 0; blending < val(ColorBlending::Total); blending++)
        {
            vulkanPipelineTextured[lightState][blending] = std::make_unique<VulkanPipelineTextured>(vulkanDevice);
            status &= vulkanPipelineTextured[lightState][blending]->setupParametred(vulkanRenderPass, lightState, (ColorBlending)blending, MSAASampleCountBit);
        }
    }

    // atmosphere pipelines
    vulkanPipelineAtmosphereColor = std::make_unique<VulkanPipelineTextured>(vulkanDevice);
    status &= vulkanPipelineAtmosphereColor->setupAtmosphere(vulkanRenderPass, MSAASampleCountBit);

    vulkanDescriptorSets = std::make_unique<VulkanDescriptorSets>(vulkanDevice, vulkanDescriptorPool, vulkanObjectBuffers);
    if (!vulkanDescriptorSets->setup(vulkanPipelineTexturedDepth.get(), vulkanPipelineTextured[val(LightState::Disabled)][val(ColorBlending::Solid)].get()))
    {
        Logger::log << "Unable to create vulkan descriptor sets" << endl;
        return false;
    }

    if (!status)
    {
        Logger::log << "Unable to create vulkan colored pipeline" << endl;
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

void VulkanPipelines::enablePipelineTexturedShadowDepth(VulkanCommandBuffer *commandBuffer, bool isMasked, bool isDoubleSided)
{
    if (isDoubleSided)
    {
        if (isMasked)
            currentPipeline = vulkanPipelineTexturedDoubleSideMaskedShadowDepth.get();
        else
            currentPipeline = vulkanPipelineTexturedDoubleSideShadowDepth.get();
    }
    else
    {
        if (isMasked)
            currentPipeline = vulkanPipelineTexturedSingleSideMaskedShadowDepth.get();
        else
            currentPipeline = vulkanPipelineTexturedSingleSideShadowDepth.get();
    }
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