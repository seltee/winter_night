#include "features/os/unix/gamepadUnix.h"

#if defined(OS_LINUX)
#include "features/logger/logger.h"
#include <cmath>
#include <cstring>
#include <libudev.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>

using namespace wne;

GamepadUnix::GamepadUnix(const char *devnode)
{
    this->devnode_ = strdup(devnode);
}

GamepadUnix::~GamepadUnix()
{
    if (devnode_)
        free(devnode_);
}

bool GamepadUnix::setup()
{
    fd = open(devnode_, O_RDONLY | O_NONBLOCK);
    if (fd < 0)
    {
        return false;
    }

    char name[256] = {};
    if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) >= 0)
    {
        this->name = std::string(name);
        this->displayName = std::string(name);
    }

    // set none blocking
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    return true;
}

bool GamepadUnix::getEvent(GamepadUnix::Event &gamepadEvent)
{
    input_event ev;
    float value;
    while (read(fd, &ev, sizeof(ev)) == sizeof(ev))
    {
        switch (ev.type)
        {
        case EV_KEY:
            gamepadEvent.type = ev.value > 0 ? EventType::ButtonPress : EventType::ButtonRelease;
            gamepadEvent.code = ev.code;
            gamepadEvent.stateAxis = 0.0f;
            return true;

        case EV_ABS:
            if (ev.code == ABS_HAT0X || ev.code == ABS_HAT0Y)
            {
                // dpad
                gamepadEvent.type = EventType::DPad;
                if (ev.code == ABS_HAT0X)
                    dpadX = ev.value;
                if (ev.code == ABS_HAT0Y)
                    dpadY = ev.value;
                gamepadEvent.code = getDPadDirection();
                gamepadEvent.stateAxis = 0.0f;
                return true;
            }
            else
            {
                // axis
                gamepadEvent.type = EventType::Axis;
                gamepadEvent.code = ev.code;
                value = ((float)ev.value) / 32767.0f;
                if (fabs(value) < deadZone)
                    value = 0.0f;
                gamepadEvent.stateAxis = value;
                return true;
            }

        case EV_SYN:
            break;
        }
    }
    return false;
}

uint32 GamepadUnix::getDPadDirection()
{
    if (dpadX == 0 && dpadY == 1)
        return 1;
    if (dpadX == 1 && dpadY == 1)
        return 2;
    if (dpadX == 1 && dpadY == 0)
        return 3;
    if (dpadX == 1 && dpadY == -1)
        return 4;
    if (dpadX == 0 && dpadY == -1)
        return 5;
    if (dpadX == -1 && dpadY == -1)
        return 6;
    if (dpadX == -1 && dpadY == 0)
        return 7;
    if (dpadX == -1 && dpadY == 1)
        return 8;
    return 0;
}

const char *GamepadUnix::getName()
{
    return name.c_str();
}

const char *GamepadUnix::getDisplayName()
{
    return displayName.c_str();
}

std::string GamepadUnix::getInternalName()
{
    return name;
}

std::string GamepadUnix::getInternalDisplayName()
{
    return displayName;
}
#endif