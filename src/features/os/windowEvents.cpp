#include "features/os/windowEvents.h"
#include <string>

using namespace wne;

void WindowEvents::pushEventKey(bool isDown, uint16 keyCode)
{
    std::lock_guard<std::mutex> lock(mutex);
    if (lastEventNum < WINDOW_EVENTS_MAX)
    {
        events[lastEventNum].type = isDown ? WindowEventType::KEY_DOWN : WindowEventType::KEY_UP;
        events[lastEventNum].key.scancode = keyCode;
        lastEventNum++;
    }
}

void WindowEvents::pushEventMouseMove(int16 shiftX, int16 shiftY)
{
    std::lock_guard<std::mutex> lock(mutex);
    if (lastEventNum < WINDOW_EVENTS_MAX)
    {
        events[lastEventNum].type = WindowEventType::MOUSE_MOVE;
        events[lastEventNum].mouseMove.shiftX = shiftX;
        events[lastEventNum].mouseMove.shiftY = shiftY;
        lastEventNum++;
    }
}

void WindowEvents::pushEventMouseClick(bool isDown, uint16 mouseButton)
{
    std::lock_guard<std::mutex> lock(mutex);
    if (lastEventNum < WINDOW_EVENTS_MAX)
    {
        events[lastEventNum].type = isDown ? WindowEventType::MOUSE_BUTTON_DOWN : WindowEventType::MOUSE_BUTTON_UP;
        events[lastEventNum].mouseButton.button = mouseButton;
        lastEventNum++;
    }
}

bool WindowEvents::getEvent(WindowEvent *event)
{
    std::lock_guard<std::mutex> lock(mutex);
    if (lastEventNum > 0)
    {
        lastEventNum--;
        memcpy(event, &events[lastEventNum], sizeof(WindowEvent));
        return true;
    }
    return false;
}
