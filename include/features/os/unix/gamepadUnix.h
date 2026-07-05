#pragma once
#include "core/platform.h"

#if defined(OS_LINUX)
#include "features/os/gamepad.h"
#include "core/core.h"
#include <string>

namespace wne
{
    class GamepadUnix : public Gamepad
    {
    public:
        enum class EventType
        {
            ButtonPress,
            ButtonRelease,
            Axis,
            DPad
        };

        struct Event
        {
            EventType type;
            uint32 code;
            float stateAxis;
        };

        GamepadUnix(const char *devnode);
        ~GamepadUnix();

        inline const char *getDevNode()
        {
            return devnode_;
        }

        bool setup();
        bool getEvent(GamepadUnix::Event &gamepadEvent);

        const char *getName() override final;
        const char *getDisplayName() override final;

    protected:
        uint32 getDPadDirection();

        std::string getInternalName();
        std::string getInternalDisplayName();

        std::string name = std::string("Gamepad");
        std::string displayName = std::string("Generic Gamepad");

        int8 dpadX = 0, dpadY = 0;

        char *devnode_ = nullptr;
        int fd = -1;
    };
};
#endif