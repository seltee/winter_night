#include "features/os/windowEvents.h"
#include <string>

using namespace wne;

void WindowEvents::pushEventKey(bool isDown, uint16 keyCode)
{
    std::lock_guard<std::mutex> lock(mutex);
    events[lastEventWrote].type = isDown ? WindowEventType::KEY_DOWN : WindowEventType::KEY_UP;
    events[lastEventWrote].key.scancode = keyCode;
    lastEventWrote = (lastEventWrote + 1) % WINDOW_EVENTS_MAX;
}

void WindowEvents::pushEventMouseMove(int16 shiftX, int16 shiftY)
{
    std::lock_guard<std::mutex> lock(mutex);
    events[lastEventWrote].type = WindowEventType::MOUSE_MOVE;
    events[lastEventWrote].mouseMove.shiftX = shiftX;
    events[lastEventWrote].mouseMove.shiftY = shiftY;
    lastEventWrote = (lastEventWrote + 1) % WINDOW_EVENTS_MAX;
}

void WindowEvents::pushEventMouseClick(bool isDown, uint16 mouseButton)
{
    std::lock_guard<std::mutex> lock(mutex);
    events[lastEventWrote].type = isDown ? WindowEventType::MOUSE_BUTTON_DOWN : WindowEventType::MOUSE_BUTTON_UP;
    events[lastEventWrote].mouseButton.button = mouseButton;
    lastEventWrote = (lastEventWrote + 1) % WINDOW_EVENTS_MAX;
}

void WindowEvents::pushEventWindowFocusChanged(bool newFocusState)
{
    std::lock_guard<std::mutex> lock(mutex);
    events[lastEventWrote].type = newFocusState ? WindowEventType::WINDOW_FOCUSED : WindowEventType::WINDOW_UNFOCUSED;
    lastEventWrote = (lastEventWrote + 1) % WINDOW_EVENTS_MAX;
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
