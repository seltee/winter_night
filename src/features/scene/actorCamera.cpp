#include "features/scene/actorCamera.h"

using namespace wne;

ActorCamera::ActorCamera(std::shared_ptr<Camera> camera)
{
    this->camera = camera;
}

std::shared_ptr<ActorCamera> ActorCamera::create(std::shared_ptr<Camera> camera)
{
    return std::make_shared<ActorCamera>(std::move(camera));
}

void ActorCamera::update(float delta)
{
    if (camera){
        camera->update();
    }
}