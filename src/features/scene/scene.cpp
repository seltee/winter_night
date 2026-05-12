#include "features/scene/scene.h"
#include "features/renderer/renderer.h"

using namespace wne;

std::shared_ptr<Scene> Scene::create()
{
    return std::make_shared<Scene>();
}

void Scene::update(float delta)
{
    for (const auto &actor : actors)
    {
        actor->update(delta);
    }
}

void Scene::render(Renderer *renderer)
{
    Matrix4x4 mVP = actorCamera ? actorCamera->getProjectionMatrix() * actorCamera->getInvModelMatrix() : Matrix4x4::identity();
    renderer->setViewProjectionMatrix(mVP);
    renderer->setAmbientColor(ambientLightColor);

    for (const auto &object : actors)
    {
        object->render(renderer);
    }
}

void Scene::addActor(std::shared_ptr<Actor> actor)
{
    actors.emplace_back(std::move(actor));
}

void Scene::setCamera(std::shared_ptr<ActorCamera> actorCamera)
{
    this->actorCamera = std::move(actorCamera);
}