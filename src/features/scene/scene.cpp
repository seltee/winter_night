#include "features/scene/scene.h"
#include "features/renderer/renderer.h"

using namespace WNE;

std::shared_ptr<Scene> Scene::create()
{
    return std::make_shared<Scene>();
}

void Scene::render(Renderer *renderer)
{
    for (const auto &object : objects)
    {
        object->render(renderer);
    }
}

void Scene::addObject(std::shared_ptr<Object> object)
{
    objects.emplace_back(std::move(object));
}
