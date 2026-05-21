#pragma once
#include "core/core.h"
#include <mutex>

#define WINDOW_EVENTS_MAX 64

namespace wne
{
    class WindowEvents
    {
    public:
        enum WindowEventType
        {
            KEY_DOWN,
            KEY_UP,
            MOUSE_BUTTON_DOWN,
            MOUSE_BUTTON_UP,
            MOUSE_MOVE,
            WINDOW_FOCUSED,
            WINDOW_UNFOCUSED,
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
        };

        struct WindowEventMouseButton
        {
            WindowEventType type;
            uint16 button;
        };

        union WindowEvent
        {
            WindowEventType type;
            WindowEventKey key;
            WindowEventMouseMove mouseMove;
            WindowEventMouseButton mouseButton;
        };

        void pushEventKey(bool isDown, uint16 keyCode);
        void pushEventMouseMove(int16 shiftX, int16 shiftY);
        void pushEventMouseClick(bool isDown, uint16 mouseButton);
        void pushEventWindowFocusChanged(bool newFocusState);

        bool getEvent(WindowEvent *event);

    protected:
        WindowEvent events[WINDOW_EVENTS_MAX];
        uint32 lastEventRead = 0;
        uint32 lastEventWrote = 0;
        std::mutex mutex;
    };
}