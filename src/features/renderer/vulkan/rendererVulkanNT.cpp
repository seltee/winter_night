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

void RendererVulkanNT::setHWND(void *hWnd)
{
    if (this->hWnd == nullptr)
    {
        this->hWnd = hWnd;
    }
}

bool RendererVulkanNT::setup(uint32 width, uint32 height)
{
    instance = new VulkanInstance();
    if (instance->initNT(hWnd, width, height))
    {
        return true;
    }
    delete instance;
    return false;
}