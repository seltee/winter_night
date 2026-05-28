#include "features/os/gamepad.h"
#include <cstring>
#include <cmath>

using namespace wne;

Gamepad::Gamepad()
{
    memset(buttonsState, 0, sizeof(buttonsState));
    memset(axisesState, 0, sizeof(axisesState));
    directionPadState = 0;
}

std::vector<ButtonChange> Gamepad::processReportButtons(GamepadReport &gamepadReport)
{
    std::vector<ButtonChange> out;
    for (uint i = 0; i < MaxButtons; i++)
    {
        if (gamepadReport.buttons[i] != buttonsState[i])
        {
            buttonsState[i] = gamepadReport.buttons[i];
            out.emplace_back(ButtonChange(i, buttonsState[i]));
        }
    }
    return out;
}

std::vector<AxisChange> Gamepad::processReportAxises(GamepadReport &gamepadReport)
{
    std::vector<AxisChange> out;
    for (uint i = 0; i < MaxButtons; i++)
    {
        float value = fabsf(gamepadReport.axises[i]) < deadZone ? 0 : gamepadReport.axises[i];
        if (value != axisesState[i])
        {
            axisesState[i] = value;
            out.emplace_back(AxisChange(i, value));
        }
    }
    return out;
}

std::vector<DirectionPadChange> Gamepad::processReportDirectionPad(GamepadReport &gamepadReport)
{
    std::vector<DirectionPadChange> out;
    if (gamepadReport.directionPad != directionPadState)
    {
        directionPadState = gamepadReport.directionPad;
        out.emplace_back(DirectionPadChange(directionPadState));
    }
    return out;
}

const char *Gamepad::getName()
{
    return "Unknown gamepad";
}

const char *Gamepad::getDisplayName()
{
    return "Unknown gamepad";
}