#include "features/renderer/vulkan/rendererVulkanNT.h"
#include "features/renderer/vulkan/vulkanMesh.h"
#include "features/renderer/vulkan/vulkanTexture.h"
#include "features/renderer/vulkan/materials/vulkanMaterial.h"
#include "features/renderer/vulkan/lights/vulkanLight.h"
#include "features/renderer/vulkan/lights/vulkanLightDirectional.h"

using namespace wne;

std::unique_ptr<RendererVulkanNT> RendererVulkanNT::create(void *hwnd)
{
    auto instance = std::unique_ptr<RendererVulkanNT>(new RendererVulkanNT());
    if (!instance->setup(hwnd))
        return nullptr;
    return instance;
}

void *RendererVulkanNT::getFrameData()
{
    return instance->getCurrentFrame();
}

void RendererVulkanNT::setAmbientColor(Vector4 &color)
{
    instance->getVulkanUtils()->getObjectBuffers()->setAmbientColor(color);
}

void RendererVulkanNT::setSyncState(bool syncEnabled)
{
    instance->setSyncState(syncEnabled);
}

bool RendererVulkanNT::getSyncState()
{
    return instance->getSyncState();
}

void RendererVulkanNT::update(float delta)
{
    for (const auto &scene : scenes)
    {
        scene->update(delta);
    }
}

void RendererVulkanNT::render()
{
    instance->startRendering();
    for (const auto &scene : scenes)
        scene->renderShadows(this);

    instance->beginDepthPass();
    for (const auto &scene : scenes)
    {
        scene->calcSceneMVP();
        scene->provideSceneMVP(this);
        scene->renderDepth(this);
    }
    instance->finishDepthPass();

    instance->beginRenderPass();
    for (const auto &scene : scenes)
    {
        scene->provideSceneMVP(this);
        scene->render(this);
    }
    instance->finishRendering();
}

void RendererVulkanNT::changeWindowSize(uint32 width, uint32 height)
{
    instance->changeSize();
}

std::shared_ptr<Mesh> RendererVulkanNT::createMesh(std::shared_ptr<Model> model)
{
    return VulkanMesh::create(model, instance->getVulkanUtils());
}

std::shared_ptr<Texture> RendererVulkanNT::createTexture(std::shared_ptr<Image> image)
{
    return VulkanTexture::create(image, instance->getVulkanUtils());
}

std::shared_ptr<Material> RendererVulkanNT::createFlatMaterial(std::shared_ptr<Texture> texture)
{
    return VulkanMaterial::createFlat(instance->getVulkanUtils(), texture);
}

std::shared_ptr<LightDirectional> RendererVulkanNT::createLightDirectional()
{
    return std::make_shared<VulkanLightDirectional>(instance->getVulkanUtils());
}

std::shared_ptr<Light> RendererVulkanNT::createLightOmni()
{
    return std::make_shared<VulkanLight>(instance->getVulkanUtils(), Light::Type::Omni);
}

std::shared_ptr<Light> RendererVulkanNT::createLightSpot()
{
    return std::make_shared<VulkanLight>(instance->getVulkanUtils(), Light::Type::Spot);
}

bool RendererVulkanNT::setup(void *hWnd)
{
    instance = VulkanInstance::create(hWnd);
    if (!instance)
    {
        return false;
    }
    return true;
}