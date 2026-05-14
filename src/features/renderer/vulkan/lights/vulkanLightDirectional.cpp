#include "features/renderer/vulkan/lights/vulkanLightDirectional.h"
#include "features/renderer/vulkan/vulkanUtils.h"
#include "features/renderer/vulkan/vulkanDepthBuffer.h"
#include "features/renderer/vulkan/vulkanDepthPass.h"
#include "features/renderer/vulkan/vulkanFrameBuffer.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <array>

using namespace wne;

VulkanLightDirectional::VulkanLightDirectional(VulkanUtils *vulkanUtils)
{
    this->vulkanUtils = vulkanUtils;
    lightId = vulkanUtils->getObjectBuffers()->getNewLightId();
}

VulkanLightDirectional::~VulkanLightDirectional()
{
    destroyShadows();
    vulkanUtils->getObjectBuffers()->freeLightId(lightId);
}

void VulkanLightDirectional::renderShadows(Scene *scene, ActorCamera *camera)
{
    for (uint i = 0; i < amountOfCascades; i++)
    {
        /*
        VulkanDepthPass *depthPass = depthPasses[i].get();
        VulkanFrameBuffer *frameBuffer = frameBuffers[i].get();

        vulkanUtils->getCurrentCommandBuffer()->beginDepthPass(depthPass, frameBuffer->getFrameBuffer());
        vulkanUtils->getCurrentCommandBuffer()->endPass();
        */
    }
}

bool VulkanLightDirectional::enableShadows(uint amountOfCascades, uint resolition)
{
    destroyShadows();
    if (amountOfCascades == 0)
        return true;

    for (uint i = 0; i < amountOfCascades; i++)
    {
        /*
        auto depthBuffer = std::make_unique<VulkanDepthBuffer>(vulkanUtils);
        if (!depthBuffer->setup(resolition, resolition, true))
        {
            std::cout << "unable to create depth buffer" << std::endl;
            return false;
        }
        std::cout << "CREATED DEPTH BUFFER" << std::endl;
        */

        // created in render pass
        /*
        auto frameBuffer = std::make_unique<VulkanFrameBuffer>(vulkanUtils->getVulkanDevice()->getDevice());
        if (!frameBuffer->setup(depthBufferCascade.get()))
            return false;
        std::cout << "CREATED FRAME BUFFER" << std::endl;
        */

        // depthBuffers.emplace_back(std::move(depthBuffer));
        // frameBuffers.emplace_back(std::move(frameBuffer));
    }
    // frameBuffers.resize(amountOfCascades);
    return true;
}

void VulkanLightDirectional::disableShadows()
{
    destroyShadows();
}

void VulkanLightDirectional::prepareForRender()
{
    vulkanUtils->getObjectBuffers()->updateLightData(
        lightId,
        type,
        affectRadius,
        cutOff,
        outerCutOff,
        position,
        realDirection,
        color);
}

void VulkanLightDirectional::destroyShadows()
{
    amountOfCascades = 0;
    depthBuffers.clear();
    frameBuffers.clear();
}