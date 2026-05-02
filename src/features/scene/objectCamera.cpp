#include "features/scene/objectCamera.h"

using namespace wne;

ObjectCamera::ObjectCamera(std::shared_ptr<Camera> camera)
{
    this->camera = camera;
}

std::shared_ptr<ObjectCamera> ObjectCamera::create(std::shared_ptr<Camera> camera)
{
    return std::make_shared<ObjectCamera>(std::move(camera));
}

void ObjectCamera::update(float delta)
{
    if (camera){
        camera->update();
    }
}