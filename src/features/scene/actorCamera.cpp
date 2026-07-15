#include "features/scene/actorCamera.h"

using namespace wne;

ActorCamera::ActorCamera(Renderer *renderer, std::shared_ptr<Camera> camera) : Actor(renderer)
{
    this->camera = camera;
}

void ActorCamera::update(float delta)
{
    if (camera)
        camera->update();
    updateClippingPlanes();
}

void ActorCamera::updateClippingPlanes()
{
    auto viewProjection = getProjectionMatrix() * getInvModelMatrix();

    leftClippingPlane = Plane(
        viewProjection[0][3] + viewProjection[0][0],
        viewProjection[1][3] + viewProjection[1][0],
        viewProjection[2][3] + viewProjection[2][0],
        viewProjection[3][3] + viewProjection[3][0]);
    leftClippingPlane.normalize();

    rightClippingPlane = Plane(
        viewProjection[0][3] - viewProjection[0][0],
        viewProjection[1][3] - viewProjection[1][0],
        viewProjection[2][3] - viewProjection[2][0],
        viewProjection[3][3] - viewProjection[3][0]);
    rightClippingPlane.normalize();

    topClippingPlane = Plane(
        viewProjection[0][3] + viewProjection[0][1],
        viewProjection[1][3] + viewProjection[1][1],
        viewProjection[2][3] + viewProjection[2][1],
        viewProjection[3][3] + viewProjection[3][1]);
    topClippingPlane.normalize();

    bottomClippingPlane = Plane(
        viewProjection[0][3] - viewProjection[0][1],
        viewProjection[1][3] - viewProjection[1][1],
        viewProjection[2][3] - viewProjection[2][1],
        viewProjection[3][3] - viewProjection[3][1]);
    bottomClippingPlane.normalize();

    nearClippingPlane = Plane(
        viewProjection[0][2],
        viewProjection[1][2],
        viewProjection[2][2],
        viewProjection[3][2]);
    nearClippingPlane.normalize();

    farClippingPlane = Plane(
        viewProjection[0][3] - viewProjection[0][2],
        viewProjection[1][3] - viewProjection[1][2],
        viewProjection[2][3] - viewProjection[2][2],
        viewProjection[3][3] - viewProjection[3][2]);
    farClippingPlane.normalize();
}

void ActorCamera::updateRenderFlag(ActorCamera *camera)
{
    isInRenderFlag = false;
}
