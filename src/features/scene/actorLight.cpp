#include "features/scene/actorLight.h"
#include "features/scene/scene.h"
#include <iostream>

using namespace wne;

ActorLight::ActorLight(Renderer *renderer, std::shared_ptr<Light> light) : Actor(renderer)
{
    this->light = light;
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

void ActorLight::update(float delta)
{
    light->updateTransformation(getModelMatrix());
    eventUpdate(delta);
}

void ActorLight::updateRenderFlag(ActorCamera *camera)
{
    isInRenderFlag = false;
}
