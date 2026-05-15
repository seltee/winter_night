#include "features/renderer/vulkan/lights/vulkanLightDirectional.h"
#include "features/renderer/vulkan/vulkanUtils.h"
#include "features/renderer/vulkan/vulkanDepthBuffer.h"
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
        VulkanRenderPass *depthPass = vulkanUtils->getCurrentDepthPass();
        VulkanFrameBuffer *frameBuffer = getFrameBuffer(i);

        vulkanUtils->getCurrentCommandBuffer()->beginDepthPass(depthPass, frameBuffer->getFrameBuffer(), resolition, resolition);

        

        vulkanUtils->getCurrentCommandBuffer()->endPass();
    }
}

bool VulkanLightDirectional::enableShadows(uint amountOfCascades, uint resolition)
{
    destroyShadows();
    if (amountOfCascades == 0)
        return true;

    for (uint i = 0; i < amountOfCascades; i++)
    {
        auto depthBuffer = std::make_unique<VulkanDepthBuffer>(vulkanUtils);
        if (!depthBuffer->setup(resolition, resolition, true))
        {
            std::cout << "unable to create depth buffer" << std::endl;
            return false;
        }
        depthBuffers.emplace_back(std::move(depthBuffer));
    }
    frameBuffers.resize(amountOfCascades);

    this->amountOfCascades = amountOfCascades;
    this->resolition = resolition;

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

VulkanFrameBuffer *VulkanLightDirectional::getFrameBuffer(int cascade)
{
    VulkanRenderPass *depthPass = vulkanUtils->getCurrentDepthPass();
    VulkanDepthBuffer *depthBuffer = depthBuffers[cascade].get();
    if (!frameBuffers[cascade])
    {
        frameBuffers[cascade] = std::make_unique<VulkanFrameBuffer>(vulkanUtils->getVulkanDevice()->getDevice());
        if (!frameBuffers[cascade]->setup(depthPass, depthBuffer))
            return nullptr;
    }
    else
    {
        // check if depth pass is still correct
        if (frameBuffers[cascade]->getDepthPass() != depthPass)
        {
            // utils have new one, recreate
            frameBuffers[cascade] = std::make_unique<VulkanFrameBuffer>(vulkanUtils->getVulkanDevice()->getDevice());
            if (!frameBuffers[cascade]->setup(depthPass, depthBuffer))
                return nullptr;
        }
    }
    return frameBuffers[cascade].get();
}