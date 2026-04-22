#include "features/renderer/vulkan/rendererVulkanNT.h"

void RendererVulkanNT::render()
{
    if (instance)
    {
        instance->render();
    }
}

void RendererVulkanNT::changeWindowSize(int width, int height)
{
    if (instance)
    {
        instance->changeSize(width, height);
    }
}

void RendererVulkanNT::setHWND(void *hWnd)
{
    if (this->hWnd == nullptr)
    {
        this->hWnd = hWnd;
    }
}

bool RendererVulkanNT::setup(int width, int height)
{
    instance = new VulkanInstance();
    if (instance->initNT(hWnd, width, height))
    {
        return true;
    }
    delete instance;
    return false;
}