#pragma once
#include "core/platform.h"

#if defined(OS_WINDOWS)
#include "features/os/gamepad.h"
#include "core/core.h"
#include <string>

namespace wne
{
    class GamepadNT : public Gamepad
    {
    public:
        GamepadNT(void *hDevice);

        const char *getName() override final;
        const char *getDisplayName() override final;

        inline void *getHandle()
        {
            return hDevice;
        }

    protected:
        std::string getInternalName();
        std::string getInternalDisplayName();

        void *hDevice = nullptr;
        uint unVendorId = 0;
        uint unProductId = 0;

        std::string name = std::string("Gamepad");
        std::string displayName = std::string("Generic Gamepad");

        char uniqueCode[16];
    };
};
#endif