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