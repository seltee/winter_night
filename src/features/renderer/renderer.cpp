#include "features/renderer/renderer.h"
#include "features/renderer/vulkan/rendererVulkanNT.h"
#include <iostream>

using namespace wne;

Renderer::~Renderer()
{
}

void Renderer::update(float delta)
{
}

void Renderer::renderStart()
{
}

void Renderer::renderScenes()
{
}

void Renderer::renderFinish()
{
}

void Renderer::changeWindowSize(int32 width, int32 height)
{
}

std::shared_ptr<Mesh> Renderer::createMesh(std::shared_ptr<Model> model)
{
    return nullptr;
}

std::shared_ptr<Texture> Renderer::createTexture(std::shared_ptr<Image> image)
{
    return nullptr;
}

std::shared_ptr<Text> Renderer::createText(std::shared_ptr<Font> font)
{
    return nullptr;
}

std::shared_ptr<Material> Renderer::createFlatMaterial(std::shared_ptr<Texture> texture)
{
    return nullptr;
}

std::shared_ptr<Material> Renderer::createAtmosphereMaterial(std::shared_ptr<Texture> texture)
{
    return nullptr;
}

void *Renderer::getFrameData()
{
    return nullptr;
}

void Renderer::setSyncState(bool syncEnabled)
{
}

bool Renderer::getSyncState()
{
    return false;
}

void Renderer::renderAtmosphereMap(std::shared_ptr<Material> atmoMaterial)
{
}

void Renderer::provideSceneData(const Vector4 &ambientColor, const Vector4 &cameraPosition, Texture *radianceMap, float radienceMapFactor)
{
}

void Renderer::addScene(std::shared_ptr<Scene> scene)
{
    scenes.push_back(std::move(scene));
}

std::shared_ptr<LightDirectional> Renderer::createLightDirectional()
{
    return nullptr;
}

std::shared_ptr<Light> Renderer::createLightOmni()
{
    return nullptr;
}

std::shared_ptr<Light> Renderer::createLightSpot()
{
    return nullptr;
}

std::shared_ptr<wne::Material> Renderer::getDefaultMaterial()
{
    return nullptr;
}

std::shared_ptr<wne::Mesh> Renderer::getDefaultPlane()
{
    return nullptr;
}

void Renderer::prepareRenderingState()
{
}

void Renderer::setMSAASampleCount(uint sampleCount)
{
}

uint Renderer::getMSAASampleCount()
{
    return 1;
}

uint Renderer::getMaxMSAASampleCount()
{
    return 1;
}

std::shared_ptr<Scene> Renderer::createScene()
{
    auto scene = wne::Scene::create(this);
    addScene(scene);
    return scene;
}