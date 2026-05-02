#include "features/scene/scene.h"
#include "features/renderer/renderer.h"

using namespace wne;

std::shared_ptr<Scene> Scene::create()
{
    return std::make_shared<Scene>();
}

void Scene::update(float delta)
{
    for (const auto &object : objects)
    {
        object->update(delta);
    }
}

void Scene::render(Renderer *renderer)
{
    Matrix4x4 mVP = objectCamera ? objectCamera->getProjectionMatrix() * objectCamera->getInvModelMatrix() : Matrix4x4::identity();
    renderer->setViewProjectionMatrix(mVP);
    for (const auto &object : objects)
    {
        object->render(renderer);
    }
}

void Scene::addObject(std::shared_ptr<Object> object)
{
    objects.emplace_back(std::move(object));
}

void Scene::setCamera(std::shared_ptr<ObjectCamera> objectCamera)
{
    this->objectCamera = std::move(objectCamera);
}