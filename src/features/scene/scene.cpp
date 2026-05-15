#include "features/scene/scene.h"
#include "features/renderer/renderer.h"
#include "features/data/light.h"
#include <algorithm>
#include <iostream>

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
    for (const auto &light : lights)
    {
        light->prepareForRender();
    }
}

void Scene::renderShadows(Renderer *renderer)
{
    if (actorCamera)
    {
        for (const auto &light : lights)
            light->renderShadows(this, actorCamera.get());
    }
}

void Scene::renderDepth(Renderer *renderer)
{
    Matrix4x4 mVP = actorCamera ? actorCamera->getProjectionMatrix() * actorCamera->getInvModelMatrix() : Matrix4x4::identity();
    renderer->setViewProjectionMatrix(mVP);
    renderer->setAmbientColor(ambientLightColor);

    // depth path
    for (const auto &object : actors)
    {
        object->renderDepth(renderer);
    }
}

void Scene::render(Renderer *renderer)
{
    // Matrix4x4 mVP = actorCamera ? actorCamera->getProjectionMatrix() * actorCamera->getInvModelMatrix() : Matrix4x4::identity();
    // renderer->setViewProjectionMatrix(mVP);
    // renderer->setAmbientColor(ambientLightColor);

    // color path
    for (const auto &object : actors)
    {
        object->renderColor(renderer);
    }
}

void Scene::addActor(std::shared_ptr<Actor> actor)
{
    actors.emplace_back(actor);
    actor->setScene(this);
}

void Scene::setCamera(std::shared_ptr<ActorCamera> actorCamera)
{
    this->actorCamera = std::move(actorCamera);
}

AffectingLights Scene::collectAffectingLights()
{
    AffectingLights affectingLights{};
    for (auto &light : lights)
    {
        if (light->getLightId() != 0xffffffff)
        {
            affectingLights.lightIds[affectingLights.lightsAmount] = light->getLightId();
            affectingLights.lightsAmount++;
        }
    }
    return affectingLights;
}

void Scene::registerLight(Light *light)
{
    if (light)
        lights.push_back(light);
}

void Scene::unregisterLight(Light *light)
{
    if (light)
        lights.erase(std::remove(lights.begin(), lights.end(), light), lights.end());
}