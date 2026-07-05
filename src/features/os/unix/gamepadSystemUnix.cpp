#include "features/os/unix/gamepadSystemUnix.h"
#include "features/logger/logger.h"
#include "features/os/window.h"
#include <libudev.h>
#include <cstring>

using namespace wne;

GamepadSystemUnix::GamepadSystemUnix(Window *window)
{
    this->window = window;
}

GamepadSystemUnix::~GamepadSystemUnix()
{
}

bool GamepadSystemUnix::setup()
{
    udev *udev = udev_new();
    if (!udev)
    {
        Logger::log << "Unable to inialize udev" << endl;
        return false;
    }

    udev_enumerate *enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "input");
    udev_enumerate_add_match_property(
        enumerate,
        "ID_INPUT_JOYSTICK",
        "1");
    udev_enumerate_scan_devices(enumerate);

    udev_list_entry *devices =
        udev_enumerate_get_list_entry(enumerate);

    udev_list_entry *entry;
    udev_list_entry_foreach(entry, devices)
    {
        const char *syspath =
            udev_list_entry_get_name(entry);

        udev_device *dev =
            udev_device_new_from_syspath(
                udev,
                syspath);

        const char *devnode = udev_device_get_devnode(dev);
        if (!devnode)
            continue;

        if (strncmp(devnode, "/dev/input/event", 16) != 0)
            continue;

        std::shared_ptr<GamepadUnix> gamepad = std::make_shared<GamepadUnix>(devnode);
        if (gamepad->setup())
        {
            gamepads.push_back(gamepad);
        }
    }

    return true;
}

void GamepadSystemUnix::update()
{
    for (auto &gamepad : gamepads)
    {
        wne::GamepadUnix::Event gamepadEvent;
        while (gamepad->getEvent(gamepadEvent))
        {
            if (gamepadEvent.type == GamepadUnix::EventType::ButtonPress)
                window->emitEventGamepadButton(gamepad, gamepadEvent.code, true);
            else if (gamepadEvent.type == GamepadUnix::EventType::ButtonRelease)
                window->emitEventGamepadButton(gamepad, gamepadEvent.code, false);
            else if (gamepadEvent.type == GamepadUnix::EventType::Axis)
                window->emitEventGamepadAxes(gamepad, gamepadEvent.code, gamepadEvent.stateAxis);
            else if (gamepadEvent.type == GamepadUnix::EventType::DPad)
                window->emitEventGamepadDirectionPad(gamepad, gamepadEvent.code);
        }
    }
}
