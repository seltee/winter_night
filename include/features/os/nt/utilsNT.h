#pragma once
#include <string>
#include <algorithm>
#include <shellscalingapi.h>
#include <windows.h>
#include <XInput.h>
#include <hidsdi.h>
#include <hidusage.h>
#include <hidpi.h>

namespace wne
{
    inline std::string convertWCharToString(const wchar_t *wcharStr)
    {
        if (!wcharStr)
            return {};

        std::mbstate_t state{};
        const wchar_t *src = wcharStr;

        std::size_t len = std::wcsrtombs(nullptr, &src, 0, &state);
        if (len == static_cast<std::size_t>(-1))
            return {};

        std::string str(len, '\0');

        state = std::mbstate_t{};
        src = wcharStr;
        std::wcsrtombs(str.data(), &src, len, &state);

        return str;
    }
};