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
            if (event.key.code == Key::KeyW)
                moveCameraForward = 1.0f;
            else if (event.key.code == Key::KeyS)
                moveCameraForward = -1.0f;
            else if (event.key.code == Key::KeyA)
                moveCameraRight = -1.0f;
            else if (event.key.code == Key::KeyD)
                moveCameraRight = 1.0f;
            else if (event.key.code == Key::KeyQ)
                moveCameraUp = -1.0f;
            else if (event.key.code == Key::KeyE)
                moveCameraUp = 1.0f;
        }
        else if (event.type == WindowEvents::WindowEventType::KEY_RELEASE)
        {
            if (event.key.code == Key::KeyW && moveCameraForward > 0)
                moveCameraForward = 0;
            else if (event.key.code == Key::KeyS && moveCameraForward < 0)
                moveCameraForward = 0;
            else if (event.key.code == Key::KeyA && moveCameraRight < 0)
                moveCameraRight = 0;
            else if (event.key.code == Key::KeyD && moveCameraRight > 0)
                moveCameraRight = 0;
            else if (event.key.code == Key::KeyQ && moveCameraUp < 0)
                moveCameraUp = 0;
            else if (event.key.code == Key::KeyE && moveCameraUp > 0)
                moveCameraUp = 0;
        }
        else if (event.type == WindowEvents::WindowEventType::GAMEPAD_AXIS)
        {
            if (event.gamepadAxis.code == 48)
            {
                moveCameraRight = event.gamepadAxis.value;
            }
            else if (event.gamepadAxis.code == 49)
            {
                moveCameraForward = -event.gamepadAxis.value;
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
            moveCameraForward = 0;
            moveCameraRight = 0;
            moveCameraUp = 0;
        }
    }

    moveForward(delta * speed * moveCameraForward);
    moveRight(delta * speed * moveCameraRight);
    moveUp(delta * speed * moveCameraUp);

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