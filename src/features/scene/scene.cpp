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
            light->renderShadows(renderer, this, actorCamera.get());
    }
}

void Scene::renderDepthShadow(Renderer *renderer)
{
    // shadow depth pass
    for (const auto &object : actors)
    {
        if (object->hasShadow())
            object->renderDepthShadow(renderer);
    }
}

void Scene::renderDepth(Renderer *renderer)
{
    // depth pass
    for (const auto &object : actors)
    {
        object->renderDepth(renderer);
    }
}

void Scene::render(Renderer *renderer)
{
    // render atmosphere if set
    if (this->atmosphereMap)
    {
        if (!atmoMaterial)
            atmoMaterial = renderer->createAtmosphereMaterial(atmosphereMap);
        renderer->renderAtmosphereMap(atmoMaterial);
    }

    std::vector<Actor *> blendingPass;
    blendingPass.reserve(actors.size());

    // color pass
    for (const auto &object : actors)
    {
        if (object->getRenderPass() == Actor::RenderPass::Main)
            object->renderColor(renderer);
        else
            blendingPass.push_back(object.get());
    }

    // blending pass
    for (const auto &object : blendingPass)
    {
        object->renderColor(renderer);
    }
}

void Scene::calcSceneMVP()
{
    mVP = actorCamera ? actorCamera->getProjectionMatrix() * actorCamera->getInvModelMatrix() : Matrix4x4::identity();
}

void Scene::provideSceneMVP(Renderer *renderer)
{
    auto state = renderer->getState();
    state->setViewProjectionMatrix(mVP);
    state->setCameraPosition(actorCamera ? actorCamera->getPosition() : Vector3{});
}

void Scene::provideSceneData(Renderer *renderer)
{
    renderer->provideSceneData(
        ambientLightColor,
        actorCamera ? Vector4(actorCamera->getPosition(), 1.0f) : Vector4(0, 0, 0, 1.0f),
        atmosphereRadiance.get());
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

void Scene::setAtmosphere(std::shared_ptr<Texture> atmosphereMap, std::shared_ptr<Texture> atmosphereRadiance)
{
    this->atmosphereMap = atmosphereMap;
    this->atmosphereRadiance = atmosphereRadiance;
    atmoMaterial = nullptr;
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