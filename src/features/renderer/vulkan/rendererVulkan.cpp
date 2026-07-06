#include "features/renderer/vulkan/rendererVulkan.h"
#include "features/renderer/vulkan/vulkanMesh.h"
#include "features/renderer/vulkan/vulkanTexture.h"
#include "features/renderer/vulkan/vulkanRendererState.h"
#include "features/renderer/vulkan/materials/vulkanMaterial.h"
#include "features/renderer/vulkan/lights/vulkanLight.h"
#include "features/renderer/vulkan/lights/vulkanLightDirectional.h"
#include "features/logger/logger.h"
#include "features/data/image.h"
#include "utils/primitives.h"
#include "core/core.h"
#include "core/math.h"
#include "engine.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <memory>

using namespace wne;

void *RendererVulkan::getFrameData()
{
    return instance->getCurrentFrame();
}

void RendererVulkan::setSyncState(bool syncEnabled)
{
    instance->setSyncState(syncEnabled);
}

bool RendererVulkan::getSyncState()
{
    return instance->getSyncState();
}

void RendererVulkan::update(float delta)
{
    instance->getVulkanUtils()->swapSets();

    instance->getVulkanUtils()->getShadowMaps()->clear();
    for (const auto &scene : scenes)
    {
        scene->update(delta);
    }
}

void RendererVulkan::renderStart()
{
    instance->startRendering();
}

void RendererVulkan::renderScenes()
{
    if (scenes.size() == 0)
        return;

    instance->beginDepthPass();
    for (const auto &scene : scenes)
    {
        scene->calcSceneMVP();
        scene->provideSceneMVP();
        scene->renderDepth();
    }
    instance->finishDepthPass();

    for (const auto &scene : scenes)
        scene->renderShadows();

    instance->getVulkanUtils()->updatePipelineShadowMaps();
    instance->beginRenderPass();

    scenes[0]->provideSceneData();
    for (const auto &scene : scenes)
    {
        scene->provideSceneMVP();
        scene->render();
    }
}

void RendererVulkan::renderFinish()
{
    instance->finishRendering();
}

void RendererVulkan::renderAtmosphereMap(std::shared_ptr<Material> atmoMaterial)
{
    static const Material::UVData uvModifier = {0.0f, 0.0f, 1.0f, 1.0f};
    auto state = getState();
    Matrix4x4 mModel = Matrix4x4::translation(state->getCameraPosition());
    Matrix3x3 mNormal = Matrix3x3::identity();
    AffectingLights lights{};
    atmoMaterial->bindColor(atmoSphereMeshId, lights, state->getViewProjectionMatrix() * mModel, mModel, mNormal, uvModifier, atmoSphere->getDataType());
    atmoSphere->render(getFrameData());
}

void RendererVulkan::provideSceneData(const Vector4 &ambientColor, const Vector4 &cameraPosition, Texture *radianceMap, float radienceMapFactor)
{
    instance->getVulkanUtils()->getObjectBuffers()->setGlobalData(ambientColor, cameraPosition, radianceMap != nullptr, radienceMapFactor);
    instance->getVulkanUtils()->setRadianceMap((VulkanTexture *)radianceMap);
}

void RendererVulkan::setMSAASampleCount(uint sampleCount)
{
    MSAASampleCount = std::min(std::max(sampleCount, (uint)1), getMaxMSAASampleCount());
    instance->updateMSAASampleCount(MSAASampleCount);
}

uint RendererVulkan::getMaxMSAASampleCount()
{
    return instance->getVulkanUtils()->getMSAAUsableSampleCount();
}

std::shared_ptr<wne::Material> RendererVulkan::getDefaultMaterial()
{
    return defaultMaterial;
}

std::shared_ptr<Mesh> RendererVulkan::getDefaultPlane()
{
    return defaultPlane;
}

void RendererVulkan::prepareRenderingState()
{
    instance->getVulkanUtils()->clearPreviousStates();
}

void RendererVulkan::changeWindowSize(int32 width, int32 height)
{
    instance->changeSize(width, height);
}

std::shared_ptr<Mesh> RendererVulkan::createMesh(std::shared_ptr<Model> model)
{
    return VulkanMesh::create(model, instance->getVulkanUtils());
}

std::shared_ptr<Texture> RendererVulkan::createTexture(std::shared_ptr<Image> image)
{
    return VulkanTexture::create(image, instance->getVulkanUtils());
}

std::shared_ptr<Text> RendererVulkan::createText(std::shared_ptr<Font> font)
{
    if (!font)
        font = Engine::getInstance()->media.getDefaultFont();
    return VulkanText::create(font, instance->getVulkanUtils());
}

std::shared_ptr<Material> RendererVulkan::createFlatMaterial(std::shared_ptr<Texture> texture)
{
    return VulkanMaterial::createFlat(instance->getVulkanUtils(), texture);
}

std::shared_ptr<Material> RendererVulkan::createAtmosphereMaterial(std::shared_ptr<Texture> texture)
{
    return VulkanMaterial::createAtmosphere(instance->getVulkanUtils(), texture);
}

std::shared_ptr<LightDirectional> RendererVulkan::createLightDirectional()
{
    return std::make_shared<VulkanLightDirectional>(instance->getVulkanUtils());
}

std::shared_ptr<Light> RendererVulkan::createLightOmni()
{
    return std::make_shared<VulkanLight>(instance->getVulkanUtils(), Light::Type::Omni);
}

std::shared_ptr<Light> RendererVulkan::createLightSpot()
{
    return std::make_shared<VulkanLight>(instance->getVulkanUtils(), Light::Type::Spot);
}

void RendererVulkan::setupDefaults()
{

    auto sphereModel = Primitives::createSphere(1.0f, 14, 10);
    atmoSphere = createMesh(sphereModel);
    atmoSphereMeshId = atmoSphere->genNewObjectId();

    const uint16 defaultWidth = 256;
    const uint16 defaultHeight = 256;
    const uint16 cellSize = 32;
    const uint16 blockSize = cellSize * 2;
    const uint16 blocksPerWidth = defaultWidth / blockSize;
    const uint16 blocksPerHeight = defaultHeight / blockSize;
    const uint32 brightColor = 0xffa2a2a2;
    const uint32 darkColor = 0xff797979;

    auto textureData = std::shared_ptr<uint8>(
        new uint8[defaultWidth * defaultHeight * 4],
        std::default_delete<uint8[]>());

    for (uint iy = 0; iy < defaultHeight; iy++)
        for (uint ix = 0; ix < defaultWidth; ix++)
            ((uint32 *)textureData.get())[iy * defaultWidth + ix] = brightColor;

    for (uint quadY = 0; quadY < blocksPerHeight; quadY++)
    {
        uint quadYShift = quadY * blockSize;
        for (uint quadX = 0; quadX < blocksPerWidth; quadX++)
        {
            uint quadXShift = quadX * blockSize;
            for (uint iy = 0; iy < cellSize; iy++)
                for (uint ix = 0; ix < cellSize; ix++)
                {
                    ((uint32 *)textureData.get())[(iy + quadYShift) * defaultWidth + (ix + quadXShift)] = darkColor;
                    ((uint32 *)textureData.get())[(iy + quadYShift + cellSize) * defaultWidth + (ix + quadXShift + cellSize)] = darkColor;
                }
        }
    }

    auto defaultTexture = createTexture(std::make_shared<Image>(textureData, defaultWidth, defaultHeight, 4));
    defaultMaterial = createFlatMaterial(defaultTexture);

    auto planeModel = Primitives::createPlane(0.5f, Vector3::forward());
    defaultPlane = createMesh(planeModel);
}