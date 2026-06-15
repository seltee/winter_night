#include "features/os/windowEvents.h"
#include <string>
#include <iostream>

using namespace wne;

WindowEvents::LastInputDevice WindowEvents::lastInputDevice = WindowEvents::LastInputDevice::Mouse;

void WindowEvents::pushEventKey(bool isDown, uint16 keyCode)
{
    std::lock_guard<std::mutex> lock(mutex);
    events[lastEventWrote].type = isDown ? WindowEventType::KEY_PRESS : WindowEventType::KEY_RELEASE;
    events[lastEventWrote].key.scancode = keyCode;
    lastEventWrote = (lastEventWrote + 1) % WINDOW_EVENTS_MAX;
    lastInputDevice = WindowEvents::LastInputDevice::Keyboard;
}

void WindowEvents::pushEventMouseMove(int16 shiftX, int16 shiftY, int16 positionX, int16 positionY)
{
    std::lock_guard<std::mutex> lock(mutex);
    events[lastEventWrote].type = WindowEventType::MOUSE_MOVE;
    events[lastEventWrote].mouseMove.shiftX = shiftX;
    events[lastEventWrote].mouseMove.shiftY = shiftY;
    events[lastEventWrote].mouseMove.positionX = positionX;
    events[lastEventWrote].mouseMove.positionY = positionY;
    lastEventWrote = (lastEventWrote + 1) % WINDOW_EVENTS_MAX;
}

void WindowEvents::pushEventMouseClick(bool isDown, uint16 mouseButton)
{
    std::lock_guard<std::mutex> lock(mutex);
    events[lastEventWrote].type = isDown ? WindowEventType::MOUSE_BUTTON_DOWN : WindowEventType::MOUSE_BUTTON_UP;
    events[lastEventWrote].mouseButton.button = mouseButton;
    lastEventWrote = (lastEventWrote + 1) % WINDOW_EVENTS_MAX;
    lastInputDevice = WindowEvents::LastInputDevice::Mouse;
}

void WindowEvents::pushEventWindowFocusChanged(bool newFocusState)
{
    std::lock_guard<std::mutex> lock(mutex);
    events[lastEventWrote].type = newFocusState ? WindowEventType::WINDOW_FOCUSED : WindowEventType::WINDOW_UNFOCUSED;
    lastEventWrote = (lastEventWrote + 1) % WINDOW_EVENTS_MAX;
}

void WindowEvents::pushEventGamepadPlugged(std::shared_ptr<Gamepad> gamepad)
{
    std::lock_guard<std::mutex> lock(mutex);
    events[lastEventWrote].type = WindowEventType::GAMEPAD_PLUGGED;
    lastEventWrote = (lastEventWrote + 1) % WINDOW_EVENTS_MAX;
}

void WindowEvents::pushEventGamepadUnplugged(std::shared_ptr<Gamepad> gamepad)
{
    std::lock_guard<std::mutex> lock(mutex);
    events[lastEventWrote].type = WindowEventType::GAMEPAD_UNPLUGGED;
    lastEventWrote = (lastEventWrote + 1) % WINDOW_EVENTS_MAX;
}

void WindowEvents::pushEventGamepadButton(std::shared_ptr<Gamepad> gamepad, uint buttonCode, bool isPressed)
{
    std::lock_guard<std::mutex> lock(mutex);
    events[lastEventWrote].type = isPressed ? WindowEventType::GAMEPAD_BUTTON_PRESS : WindowEventType::GAMEPAD_BUTTON_RELEASE;
    events[lastEventWrote].gamepadButton.button = buttonCode;
    lastEventWrote = (lastEventWrote + 1) % WINDOW_EVENTS_MAX;
    lastInputDevice = WindowEvents::LastInputDevice::Gamepad;
}

void WindowEvents::pushEventGamepadAxis(std::shared_ptr<Gamepad> gamepad, uint axisCode, float value)
{
    std::lock_guard<std::mutex> lock(mutex);
    events[lastEventWrote].type = WindowEventType::GAMEPAD_AXIS;
    events[lastEventWrote].gamepadAxis.value = value;
    events[lastEventWrote].gamepadAxis.code = axisCode;
    lastEventWrote = (lastEventWrote + 1) % WINDOW_EVENTS_MAX;
}

void WindowEvents::pushEventGamepadDirectionPad(std::shared_ptr<Gamepad> gamepad, uint value)
{
    std::lock_guard<std::mutex> lock(mutex);
    events[lastEventWrote].type = WindowEventType::GAMEPAD_DIRECTION_PAD;
    events[lastEventWrote].gamepadDirectionPad.direction = value;
    lastEventWrote = (lastEventWrote + 1) % WINDOW_EVENTS_MAX;
    lastInputDevice = WindowEvents::LastInputDevice::Gamepad;
}

bool WindowEvents::getEvent(WindowEvent *event)
{
    std::lock_guard<std::mutex> lock(mutex);
    if (lastEventWrote != lastEventRead)
    {
        memcpy(event, &events[lastEventRead], sizeof(WindowEvent));
        lastEventRead = (lastEventRead + 1) % WINDOW_EVENTS_MAX;
        return true;
    }
    return false;
}
