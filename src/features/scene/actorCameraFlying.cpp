#include "features/scene/actorCameraFlying.h"
#include "features/os/windowEvents.h"
#include "features/os/window.h"
#include "core/math.h"
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace wne;

ActorCameraFlying::ActorCameraFlying(Renderer *renderer, std::shared_ptr<Camera> camera, std::shared_ptr<Window> eventWindow) : ActorCamera(renderer, camera)
{
    eventsSubscription = eventWindow->subscribe();
}

ActorCameraFlying::~ActorCameraFlying()
{
}

void ActorCameraFlying::update(float delta)
{
    if (camera)
        camera->update();

    if (!eventsSubscription)
        return;

    WindowEvents::WindowEvent event;
    while (eventsSubscription->getEvent(&event))
    {
        if (event.type == WindowEvents::WindowEventType::KEY_PRESS)
        {
            if (event.key.scancode == 87)
                moveForward = 1.0f;
            else if (event.key.scancode == 83)
                moveForward = -1.0f;
            else if (event.key.scancode == 65)
                moveRight = -1.0f;
            else if (event.key.scancode == 68)
                moveRight = 1.0f;
            else if (event.key.scancode == 81)
                moveUp = -1.0f;
            else if (event.key.scancode == 69)
                moveUp = 1.0f;
        }
        else if (event.type == WindowEvents::WindowEventType::KEY_RELEASE)
        {
            if (event.key.scancode == 87 && moveForward > 0)
                moveForward = 0;
            else if (event.key.scancode == 83 && moveForward < 0)
                moveForward = 0;
            else if (event.key.scancode == 65 && moveRight < 0)
                moveRight = 0;
            else if (event.key.scancode == 68 && moveRight > 0)
                moveRight = 0;
            else if (event.key.scancode == 81 && moveUp < 0)
                moveUp = 0;
            else if (event.key.scancode == 69 && moveUp > 0)
                moveUp = 0;
        }
        else if (event.type == WindowEvents::WindowEventType::GAMEPAD_AXIS)
        {
            if (event.gamepadAxis.code == 48)
            {
                moveRight = event.gamepadAxis.value;
            }
            else if (event.gamepadAxis.code == 49)
            {
                moveForward = -event.gamepadAxis.value;
            }
            else if (event.gamepadAxis.code == 51)
            {
                rotateYaw = event.gamepadAxis.value * 0.016f;
            }
            else if (event.gamepadAxis.code == 52)
            {
                rotatePitch = event.gamepadAxis.value * 0.016f;
            }
        }
        else if (event.type == WindowEvents::WindowEventType::MOUSE_MOVE)
        {
            addCameraRotation(
                (float)event.mouseMove.shiftX * 0.009f,
                (float)event.mouseMove.shiftY * 0.009f);
        }
        else if (event.type == WindowEvents::WindowEventType::WINDOW_FOCUSED || event.type == WindowEvents::WindowEventType::WINDOW_UNFOCUSED)
        {
            moveForward = 0;
            moveRight = 0;
            moveUp = 0;
        }
    }

    Vector3 forward = rotation * Vector3::forward();
    Vector3 right = rotation * Vector3::right();
    Vector3 up = rotation * Vector3::up();

    position += forward * delta * speed * moveForward;
    position += right * delta * speed * moveRight;
    position += up * delta * speed * moveUp;
    if (rotateYaw != 0.0f || rotatePitch != 0.0f)
        addCameraRotation(rotateYaw, rotatePitch);
    isDirtyFlag = true;

    eventUpdate(delta);

    updateClippingPlanes();
}

void ActorCameraFlying::addCameraRotation(float yaw, float pitch)
{
    this->yaw += yaw;     // left/right
    this->pitch += pitch; // up/down

    this->yaw = wrap2Pi(this->yaw);

    this->pitch = std::clamp(this->pitch, -1.4f, 1.4f);

    Quat qPitch = Quat::aroundWorldX(this->pitch);
    Quat qYaw = Quat::aroundWorldY(this->yaw);
    rotation = normalize(qYaw * qPitch);
}