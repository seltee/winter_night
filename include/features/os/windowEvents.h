#pragma once
#include "core/core.h"
#include "features/os/gamepad.h"
#include <mutex>
#include <memory>

#define WINDOW_EVENTS_MAX 64

namespace wne
{
    class WindowEvents
    {
    public:
        enum WindowEventType
        {
            KEY_PRESS,
            KEY_RELEASE,
            MOUSE_BUTTON_DOWN,
            MOUSE_BUTTON_UP,
            MOUSE_MOVE,
            WINDOW_FOCUSED,
            WINDOW_UNFOCUSED,
            GAMEPAD_PLUGGED,
            GAMEPAD_UNPLUGGED,
            GAMEPAD_BUTTON_PRESS,
            GAMEPAD_BUTTON_RELEASE,
            GAMEPAD_AXIS,
            GAMEPAD_DIRECTION_PAD
        };

        struct WindowEventKey
        {
            WindowEventType type;
            uint16 scancode;
        };

        struct WindowEventMouseMove
        {
            WindowEventType type;
            int16 shiftX;
            int16 shiftY;
            int16 positionX;
            int16 positionY;
        };

        struct WindowEventMouseButton
        {
            WindowEventType type;
            uint16 button;
        };

        struct WindowEventGamepadButton
        {
            WindowEventType type;
            uint16 button;
        };

        struct WindowEventGamepadAxis
        {
            WindowEventType type;
            uint16 code;
            float value;
        };

        struct WindowEventGamepadDirectionPad
        {
            WindowEventType type;
            uint direction;
        };

        union WindowEvent
        {
            WindowEventType type;
            WindowEventKey key;
            WindowEventMouseMove mouseMove;
            WindowEventMouseButton mouseButton;
            WindowEventGamepadButton gamepadButton;
            WindowEventGamepadAxis gamepadAxis;
            WindowEventGamepadDirectionPad gamepadDirectionPad;
        };

        void pushEventKey(bool isPressed, uint16 keyCode);
        void pushEventMouseMove(int16 shiftX, int16 shiftY, int16 positionX, int16 positionY);
        void pushEventMouseClick(bool isPressed, uint16 mouseButton);
        void pushEventWindowFocusChanged(bool newFocusState);
        void pushEventGamepadPlugged(std::shared_ptr<Gamepad> gamepad);
        void pushEventGamepadUnplugged(std::shared_ptr<Gamepad> gamepad);
        void pushEventGamepadButton(std::shared_ptr<Gamepad> gamepad, uint buttonCode, bool isPressed);
        void pushEventGamepadAxis(std::shared_ptr<Gamepad> gamepad, uint axisCode, float value);
        void pushEventGamepadDirectionPad(std::shared_ptr<Gamepad> gamepad, uint value);

        bool getEvent(WindowEvent *event);

    protected:
        WindowEvent events[WINDOW_EVENTS_MAX];
        uint32 lastEventRead = 0;
        uint32 lastEventWrote = 0;
        std::mutex mutex;
    };
}