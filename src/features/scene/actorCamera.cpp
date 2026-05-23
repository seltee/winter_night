#include "features/scene/actorCamera.h"

using namespace wne;

ActorCamera::ActorCamera(Renderer *renderer, std::shared_ptr<Camera> camera) : Actor(renderer)
{
    this->camera = camera;
}

void ActorCamera::update(float delta)
{
    if (camera){
        camera->update();
    }
}