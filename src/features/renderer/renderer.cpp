#include "features/renderer/renderer.h"
#include "features/renderer/vulkan/rendererVulkanNT.h"

using namespace wne;

Renderer::~Renderer()
{
}

void Renderer::update(float delta)
{
}

void Renderer::render()
{
}

void Renderer::changeWindowSize(uint32 width, uint32 height)
{
}

std::shared_ptr<Mesh> Renderer::createMesh(std::shared_ptr<Model> model)
{
    return nullptr;
}

std::shared_ptr<Texture> Renderer::createTexture(std::shared_ptr<Image> image)
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

void Renderer::setModelMatrix(const Matrix4x4 &model)
{
}

void Renderer::setViewProjectionMatrix(Matrix4x4 &VP)
{
}

void Renderer::addScene(std::shared_ptr<Scene> scene)
{
    scenes.push_back(std::move(scene));
}