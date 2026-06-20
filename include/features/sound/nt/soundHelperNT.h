// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "core/core.h"

#if defined(OS_WINDOWS)

#include <windows.h>
#include <dsound.h>
#include <string>

struct AudioDeviceWindows
{
    std::string description;
    LPGUID lpGuid;
};

inline std::string convertToString(LPCWSTR wideString)
{
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wideString, -1, NULL, 0, NULL, NULL);
    if (bufferSize == 0)
        return "empty_string";

    std::string narrowString(bufferSize, 0);
    WideCharToMultiByte(CP_UTF8, 0, wideString, -1, &narrowString[0], bufferSize, NULL, NULL);

    // Remove the null terminator from the string
    narrowString.resize(bufferSize - 1);

    return narrowString;
}

inline std::string getAudioError(const std::string &systemMessage, HRESULT result)
{
    if (result == DSERR_ALLOCATED)
        return systemMessage + ": because resources, such as a priority level, were already in use by another caller\n";
    else if (result == DSERR_INVALIDPARAM)
        return systemMessage + ": because an invalid parameter was passed\n";
    else if (result == DSERR_CONTROLUNAVAIL)
        return systemMessage + ": the buffer control (volume, pan, and so on) requested by the caller is not available. Controls must be specified when the buffer is created, using the dwFlags member of DSBUFFERDESC.\n";
    else if (result == DSERR_NOAGGREGATION)
        return systemMessage + ": because the object does not support aggregation\n";
    else if (result == DSERR_NODRIVER)
        return systemMessage + ": because no sound driver is available for use, or the given GUID is not a valid DirectSound device ID\n";
    else if (result == DSERR_OUTOFMEMORY)
        return systemMessage + ": because the DirectSound subsystem could not allocate sufficient memory to complete the caller's request\n";
    else if (result == DSERR_BADFORMAT)
        return systemMessage + ": because of the bad format\n";
    else if (result == DSERR_UNSUPPORTED)
        return systemMessage + ": the function called is not supported at this time\n";
    else if (result == DSERR_BUFFERLOST)
        return systemMessage + ": The buffer memory has been lost and must be restored\n";
    else if (result == DSERR_INVALIDCALL)
        return systemMessage + ": This function is not valid for the current state of this object\n";
    else if (result == DSERR_PRIOLEVELNEEDED)
        return systemMessage + ": A cooperative level of DSSCL_PRIORITY or higher is required.\n";
    else
        return systemMessage + ": with unknown error\n";
}

#endif