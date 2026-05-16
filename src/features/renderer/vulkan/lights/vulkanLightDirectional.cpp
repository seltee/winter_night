#include "features/renderer/vulkan/lights/vulkanLightDirectional.h"
#include "features/renderer/vulkan/vulkanTexture.h"
#include "features/renderer/vulkan/vulkanUtils.h"
#include "features/renderer/vulkan/vulkanDepthBuffer.h"
#include "features/renderer/vulkan/vulkanFrameBuffer.h"
#include "features/renderer/vulkan/vulkanRendererState.h"
#include "features/renderer/vulkan/rendererVulkanNT.h"
#include "features/renderer/renderer.h"
#include "features/scene/scene.h"
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

void VulkanLightDirectional::renderShadows(Renderer *renderer, Scene *scene, ActorCamera *camera)
{
    for (uint i = 0; i < amountOfCascades; i++)
    {
        VulkanRenderPass *depthPass = vulkanUtils->getCurrentDepthPass();
        VulkanFrameBuffer *frameBuffer = getFrameBuffer(i);

        Matrix4x4 mProjection = makeOrthographicProjectionMatrix(-60.0f, 60.0f, 60.0f, -60.0f, 0.0f, 80.0f);

        Vector3 projectionPosition = position.xyz() + realDirection.xyz() * 40.0f;
        Matrix4x4 model = Matrix4x4::translation(projectionPosition);
        model = model * Matrix4x4(lookAt(projectionPosition, position.xyz()));
        Matrix4x4 invModelMatrix = inverse(model);
        Matrix4x4 mVP = mProjection * invModelMatrix;

        // current queue building state
        auto state = (VulkanRendererState *)renderer->getState();
        state->setViewProjectionMatrix(mVP);
        state->setVulkanLightCascadeData(cascades[i].get());

        vulkanUtils->getCurrentCommandBuffer()->beginDepthPass(depthPass, frameBuffer->getFrameBuffer(), resolition, resolition);
        scene->renderDepthShadow(renderer);
        vulkanUtils->getCurrentCommandBuffer()->endPass();
    }
}

bool VulkanLightDirectional::enableShadows(uint amountOfCascades, uint resolition)
{
    destroyShadows();
    if (amountOfCascades == 0)
        return true;

    cascades.resize(amountOfCascades);
    for (uint i = 0; i < amountOfCascades; i++)
    {
        cascades[i] = std::make_unique<VulkanLightCascadeData>(vulkanUtils);
        if (!cascades[i]->setup(resolition))
        {
            std::cout << "unable to create depth buffer" << std::endl;
            return false;
        }
    }

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
    if (amountOfCascades > 0)
        shadowId = vulkanUtils->getShadowMaps()->registerShadowMap(cascades[0]->getDepthBuffer());
    

    vulkanUtils->getObjectBuffers()->updateLightData(
        lightId,
        type,
        affectRadius,
        cutOff,
        outerCutOff,
        shadowId,
        amountOfCascades,
        position,
        realDirection,
        color);
}

std::shared_ptr<Texture> VulkanLightDirectional::getCascadeAsTexture(int numOfCascade)
{
    auto depthBuffer = cascades[numOfCascade]->getDepthBuffer();

    std::shared_ptr<VulkanTexture> texture = std::make_shared<VulkanTexture>(vulkanUtils);
    if (!texture->setup(depthBuffer->getDepthImageView()))
    {
        std::cout << "Unable to create texture from image view for shadow cascade" << std::endl;
        return nullptr;
    }
    return texture;
}

void VulkanLightDirectional::destroyShadows()
{
    amountOfCascades = 0;
    cascades.clear();
}

VulkanFrameBuffer *VulkanLightDirectional::getFrameBuffer(int cascade)
{
    VulkanRenderPass *depthPass = vulkanUtils->getCurrentDepthPass();
    return cascades[cascade]->getFrameBuffer(depthPass);
}