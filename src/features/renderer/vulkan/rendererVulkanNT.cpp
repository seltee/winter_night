#include "features/renderer/vulkan/rendererVulkanNT.h"
#include "features/renderer/vulkan/vulkanMesh.h"
#include "features/renderer/vulkan/vulkanTexture.h"
#include "features/renderer/vulkan/vulkanMaterial.h"

using namespace wne;

void *RendererVulkanNT::getFrameData()
{
    return instance->getCurrentFrame();
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
    {
        scene->render(this);
    }
    instance->finishRendering();
}

void RendererVulkanNT::changeWindowSize(uint32 width, uint32 height)
{
    instance->changeSize(width, height);
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

bool RendererVulkanNT::setup(void *hWnd, uint32 width, uint32 height)
{
    instance = VulkanInstance::create(hWnd, width, height);
    if (!instance)
    {
        return false;
    }
    return true;
}