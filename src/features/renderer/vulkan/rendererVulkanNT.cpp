#include "features/renderer/vulkan/rendererVulkanNT.h"
#include "features/renderer/vulkan/vulkanModelRender.h"

using namespace WNE;

void RendererVulkanNT::render()
{
    if (instance)
    {
        instance->render();
    }
}

void RendererVulkanNT::changeWindowSize(uint32 width, uint32 height)
{
    if (instance)
    {
        instance->changeSize(width, height);
    }
}

ModelRender *RendererVulkanNT::createFromModel(Model *model)
{
    return nullptr;
}

ModelRender *createFromModel(Model *model)
{
    return VulkanModelRender::fromModel(model);
}

bool RendererVulkanNT::setup(void *hWnd, uint32 width, uint32 height)
{
    instance = VulkanInstance::create(hWnd, width, height);
    if (!instance){
        return false;
    }
    return true;
}