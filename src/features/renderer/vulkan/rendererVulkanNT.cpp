#include "features/renderer/vulkan/rendererVulkanNT.h"
#include "features/renderer/vulkan/vulkanMesh.h"

using namespace WNE;

void *RendererVulkanNT::getFrameData()
{
    return instance->getCurrentFrame();
}

void RendererVulkanNT::render()
{
    if (instance)
    {
        instance->startRendering();
        for (const auto & scene : scenes){
            scene->render(this);
        }
        instance->finishRendering();
    }
}

void RendererVulkanNT::changeWindowSize(uint32 width, uint32 height)
{
    if (instance)
    {
        instance->changeSize(width, height);
    }
}

std::shared_ptr<Mesh> RendererVulkanNT::createMesh(std::shared_ptr<Model> model)
{
    if (instance)
    {
        return VulkanMesh::create(model, instance->getVulkanDevice());
    }
    return nullptr;
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