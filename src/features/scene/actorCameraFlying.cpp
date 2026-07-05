#include "features/scene/actorCameraFlying.h"
#include "features/os/windowEvents.h"
#include "features/os/window.h"
#include "features/logger/logger.h"
#include "features/os/gamepad.h"
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
            if (event.key.code == Key::W)
                moveCameraForward = 1.0f;
            else if (event.key.code == Key::S)
                moveCameraForward = -1.0f;
            else if (event.key.code == Key::A)
                moveCameraRight = -1.0f;
            else if (event.key.code == Key::D)
                moveCameraRight = 1.0f;
            else if (event.key.code == Key::Q)
                moveCameraUp = -1.0f;
            else if (event.key.code == Key::E)
                moveCameraUp = 1.0f;
        }
        else if (event.type == WindowEvents::WindowEventType::KEY_RELEASE)
        {
            if (event.key.code == Key::W && moveCameraForward > 0)
                moveCameraForward = 0;
            else if (event.key.code == Key::S && moveCameraForward < 0)
                moveCameraForward = 0;
            else if (event.key.code == Key::A && moveCameraRight < 0)
                moveCameraRight = 0;
            else if (event.key.code == Key::D && moveCameraRight > 0)
                moveCameraRight = 0;
            else if (event.key.code == Key::Q && moveCameraUp < 0)
                moveCameraUp = 0;
            else if (event.key.code == Key::E && moveCameraUp > 0)
                moveCameraUp = 0;
        }
        else if (event.type == WindowEvents::WindowEventType::GAMEPAD_AXIS)
        {
            if (event.gamepadAxis.code == GAMEPAD_AXIS_LEFT_V)
                moveCameraRight = event.gamepadAxis.value;
            else if (event.gamepadAxis.code == GAMEPAD_AXIS_LEFT_H)
                moveCameraForward = -event.gamepadAxis.value;
            else if (event.gamepadAxis.code == GAMEPAD_AXIS_RIGHT_V)
                rotateYaw = event.gamepadAxis.value * 0.016f;
            else if (event.gamepadAxis.code == GAMEPAD_AXIS_RIGHT_H)
                rotatePitch = event.gamepadAxis.value * 0.016f;
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