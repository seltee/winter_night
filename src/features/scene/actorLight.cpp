#include "features/scene/actorLight.h"
#include "features/scene/scene.h"
#include <iostream>

using namespace wne;

ActorLight::ActorLight(std::shared_ptr<Light> light)
{
    this->light = light;
}

std::shared_ptr<ActorLight> ActorLight::create(std::shared_ptr<Light> light)
{
    return std::make_shared<ActorLight>(std::move(light));
}

void ActorLight::eventSetScene(Scene *oldScene, Scene *newScene)
{
    if (light)
    {
        if (oldScene)
            oldScene->unregisterLight(light.get());
        if (newScene)
            newScene->registerLight(light.get());
    }
}