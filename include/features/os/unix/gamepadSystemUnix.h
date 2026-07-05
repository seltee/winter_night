#pragma once
#include "core/platform.h"

#if defined(OS_LINUX)
#include "features/os/unix/gamepadUnix.h"
#include "core/core.h"
#include <string>
#include <vector>
#include <memory>

struct udev_monitor;

namespace wne
{
    class Window;

    class GamepadSystemUnix
    {
    public:
        GamepadSystemUnix(Window *window);
        ~GamepadSystemUnix();

        bool setup();
        void update();

    protected:
        std::vector<std::shared_ptr<GamepadUnix>> gamepads;
        Window *window = nullptr;

        udev_monitor *monitor_ = nullptr;
        int monitorFd = -1;
    };
};
#endif