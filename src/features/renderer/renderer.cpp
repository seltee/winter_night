#include "features/renderer/renderer.h"
#include "features/renderer/vulkan/rendererVulkanNT.h"
#include <iostream>
#include <iterator>

using namespace wne;

Renderer::~Renderer()
{
}

void Renderer::update(float delta)
{
    updateDebugVisuals(delta);
}

void Renderer::renderStart()
{
}

void Renderer::renderScenes()
{
}

void Renderer::renderDebug()
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

std::shared_ptr<MeshCollection> Renderer::createMeshCollection(std::vector<std::shared_ptr<Model>> models)
{
    return nullptr;
}

std::shared_ptr<MeshArmature> Renderer::createMeshArmature(std::shared_ptr<Armature> armature)
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

std::shared_ptr<MaterialFlat> Renderer::createFlatMaterial()
{
    return nullptr;
}

std::shared_ptr<MaterialFlat> Renderer::createFlatMaterial(std::shared_ptr<Texture> texture)
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

std::shared_ptr<Material> Renderer::getDefaultMaterial()
{
    return nullptr;
}

std::shared_ptr<Mesh> Renderer::getDefaultPlane()
{
    return nullptr;
}

std::shared_ptr<Mesh> Renderer::getDefaultCube()
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

void Renderer::addDebugLine(const Vector3 &from, const Vector3 &to, Renderer::DebugColor color, float timer, bool onTop)
{
    std::lock_guard<std::mutex> lock(mutexDebugData);
    debugLineData.emplace_back(DebugLineData({from, to, timer, color, onTop, false}));
}

void Renderer::addDebugCube(const Vector3 &position, Renderer::DebugColor color, float timer, bool onTop)
{
    std::lock_guard<std::mutex> lock(mutexDebugData);
    debugCubeData.emplace_back(DebugCubeData({position, timer, color, onTop, false}));
}

void Renderer::updateDebugVisuals(float delta)
{
    std::lock_guard<std::mutex> lock(mutexDebugData);

    std::erase_if(debugLineData, [&](DebugLineData &node)
                  { 
                    node.removeTimer -= delta;
                    return node.removeTimer<0.0f && node.oneFrameShown; });

    std::erase_if(debugCubeData, [&](DebugCubeData &node)
                  { 
                    node.removeTimer -= delta;
                    return node.removeTimer<0.0f && node.oneFrameShown; });
}