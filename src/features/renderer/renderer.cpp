#include "features/renderer/renderer.h"
#include "features/renderer/vulkan/rendererVulkanNT.h"

Renderer::~Renderer()
{
}

void Renderer::render()
{
}

void Renderer::changeWindowSize(int width, int height)
{
}

Renderer *Renderer::createRendererNT(void *hWnd, int width, int height)
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

bool Renderer::setup(int width, int height)
{
    return false;
}