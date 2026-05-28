#pragma once
#include "core/core.h"
#include <vector>

namespace wne
{
    struct GamepadReport;
    struct ButtonChange
    {
        uint16 keyId;
        bool isPressed;
    };

    struct AxisChange
    {
        uint16 axisId;
        float value;
    };

    struct DirectionPadChange
    {
        uint value;
    };

    class Gamepad
    {
    public:
        Gamepad();
        std::vector<ButtonChange> processReportButtons(GamepadReport &gamepadReport);
        std::vector<AxisChange> processReportAxises(GamepadReport &gamepadReport);
        std::vector<DirectionPadChange> processReportDirectionPad(GamepadReport &gamepadReport);

        virtual const char *getName();
        virtual const char *getDisplayName();

        inline void setUnplugged()
        {
            isPluggedFlag = false;
        }

        inline bool isPlugged()
        {
            return isPluggedFlag;
        }

        inline float getDeadZone()
        {
            return deadZone;
        }

        inline void setDeadZone(float deadZone)
        {
            this->deadZone = deadZone;
        }

        static constexpr uint MaxButtons = 128;
        static constexpr uint MaxAxises = 128;

    protected:
        bool isPluggedFlag = true;
        float deadZone = 0.2f;

        bool buttonsState[Gamepad::MaxButtons];
        float axisesState[Gamepad::MaxAxises];
        uint directionPadState;
    };

    struct GamepadReport
    {
        bool buttons[Gamepad::MaxButtons];
        float axises[Gamepad::MaxAxises];
        uint directionPad;
    };

};
