#include "features/scene/scene.h"
#include "features/renderer/renderer.h"
#include "features/data/light.h"
#include "engine.h"
#include <algorithm>
#include <iostream>

using namespace wne;

void _actorsUpdate(float delta, uint from, uint to, std::vector<std::shared_ptr<wne::Actor>> *actors, ActorCamera *actorCamera);

Scene::Scene(Renderer *renderer)
{
    this->renderer = renderer;
}

std::shared_ptr<Scene> Scene::create(Renderer *renderer)
{
    return std::make_shared<Scene>(renderer);
}

void Scene::update(float delta)
{
    // update all actors multithreaded
    const uint actorsPerJob = 16;
    for (uint i = 0; i < actors.size(); i += actorsPerJob)
    {
        uint to = std::min((uint)(i + actorsPerJob), (uint)actors.size());
        auto pActors = &actors;
        ActorCamera *currentCamera = actorCamera.get();
        Engine::getInstance()->getJobQueue().queueJob([delta, i, to, pActors, currentCamera]
                                                      { _actorsUpdate(delta, i, to, pActors, currentCamera); });
    }
    Engine::getInstance()->getJobQueue().waitJobs();

    // remove destroyed actors
    for (uint i = 0; i < actors.size();)
    {
        if (actors[i]->isDestroyed())
        {
            actors[i]->eventDestroyed();
            actors[i] = actors[actors.size() - 1];
            actors.pop_back();
        }
        else
            i++;
    }

    // prepare light
    if (actorCamera)
    {
        for (const auto &light : lights)
        {
            light->prepareForRender(actorCamera->getPosition());
        }
    }
}

void Scene::renderShadows()
{
    if (actorCamera)
    {
        for (const auto &light : lights)
            light->renderShadows(renderer, this, actorCamera.get());
    }
}

void Scene::renderDepthShadow(Vector3 &lightPosition)
{
    // shadow depth pass
    for (const auto &object : actors)
    {
        if (object->hasShadow() && object->isVisible())
            object->renderDepthShadow(lightPosition);
    }
}

void Scene::renderDepth()
{
    // depth pass
    for (const auto &object : actors)
    {
        if (object->isRendered() && object->isVisible())
            object->renderDepth();
    }
}

void Scene::render()
{
    renderer->prepareRenderingState();

    // render atmosphere if set
    if (this->atmosphereMap)
    {
        if (!atmoMaterial)
            atmoMaterial = renderer->createAtmosphereMaterial(atmosphereMap);
        renderer->renderAtmosphereMap(atmoMaterial);
    }

    blendingPass.clear();
    blendingPass.reserve(actors.size());

    // color pass
    for (const auto &object : actors)
    {
        if (object->isRendered() && object->isVisible())
        {
            if (object->getRenderPass() == Actor::RenderPass::Main)
                object->renderColor();
            else
                blendingPass.push_back(object.get());
        }
    }

    // blending pass
    for (const auto &object : blendingPass)
    {
        object->renderColor();
    }
}

void Scene::calcSceneMVP()
{
    mVP = actorCamera ? actorCamera->getProjectionMatrix() * actorCamera->getInvModelMatrix() : Matrix4x4::identity();
}

void Scene::provideSceneMVP()
{
    auto state = renderer->getState();
    state->setViewProjectionMatrix(mVP);
    state->setCameraPosition(actorCamera ? actorCamera->getPosition() : Vector3{});
}

void Scene::provideSceneData()
{
    renderer->provideSceneData(
        ambientLightColor,
        actorCamera ? Vector4(actorCamera->getPosition(), 1.0f) : Vector4(0, 0, 0, 1.0f),
        atmosphereRadiance.get(), atmosphereRadianceFactor);
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

void Scene::setAtmosphere(std::shared_ptr<Texture> atmosphereMap, std::shared_ptr<Texture> atmosphereRadiance, float atmosphereRadianceFactor)
{
    this->atmosphereMap = atmosphereMap;
    this->atmosphereRadiance = atmosphereRadiance;
    this->atmosphereRadianceFactor = atmosphereRadianceFactor;
    atmoMaterial = nullptr;
}

AffectingLights Scene::collectAffectingLights(const Vector3 &point, float radius)
{
    std::vector<std::pair<uint32, float>> lightsList;
    for (auto &light : lights)
    {
        if (light->getLightId() != 0xffffffff)
        {
            lightsList.emplace_back(light->getLightId(), distance(point, light->getPosition().xyz()));
        }
    }

    std::sort(lightsList.begin(), lightsList.end(), [](const std::pair<uint32, float> &a, const std::pair<uint32, float> &b)
              { return a.second < b.second; });

    AffectingLights affectingLights{};
    for (auto &lightPart : lightsList)
    {
        affectingLights.lightIds[affectingLights.lightsAmount] = lightPart.first;
        affectingLights.lightsAmount++;
        if (affectingLights.lightsAmount >= 12) // max lights reached
            break;
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

void _actorsUpdate(float delta, uint from, uint to, std::vector<std::shared_ptr<wne::Actor>> *actors, ActorCamera *actorCamera)
{
    for (uint i = from; i < to; i++)
    {
        (*actors)[i]->update(delta);
        (*actors)[i]->updateRenderFlag(actorCamera);
    }
}