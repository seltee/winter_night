#include "features/renderer/renderer.h"
#include "features/renderer/vulkan/rendererVulkanNT.h"

using namespace WNE;

Renderer::~Renderer()
{
}

void Renderer::render()
{
}

void Renderer::changeWindowSize(uint32 width, uint32 height)
{
}

ModelRender *Renderer::createFromModel(Model *model)
{
    return nullptr;
}

Renderer *Renderer::createRendererNT(void *hWnd, uint32 width, uint32 height)
{
    RendererVulkanNT *rendererVulkan = new RendererVulkanNT();
    rendererVulkan->setHWND(hWnd);
    if (rendererVulkan->setup(width, height))
    {
        return rendererVulkan;
    }
    delete rendererVulkan;
    return nullptr;
}

bool Renderer::setup(uint32 width, uint32 height)
{
    return false;
}