#include "features/os/nt/gamepadNT.h"
#include "features/os/nt/utilsNT.h"
#include <iostream>
#include <windows.h>
#include <setupapi.h>
#include <hidsdi.h>
#include <iostream>
#include <tchar.h>

using namespace wne;

static const GUID GUID_DEVINTERFACE_HID = {0x4d1e55b2, 0xf16f, 0x11cf, {0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30}};

struct UniqueCode
{
    unsigned int unVendorId;
    unsigned int unProductId;
};

GamepadNT::GamepadNT(void *hDevice)
{
    this->hDevice = hDevice;

    RID_DEVICE_INFO rdi;
    rdi.cbSize = sizeof(rdi);
    unsigned int rdiSize = sizeof(rdi);
    GetRawInputDeviceInfo(hDevice, RIDI_DEVICEINFO, &rdi, &rdiSize);

    unVendorId = rdi.hid.dwVendorId;
    unProductId = rdi.hid.dwProductId;
    name = getInternalName();
    displayName = getInternalDisplayName();

    memset(uniqueCode, 0xff, 16);
    reinterpret_cast<UniqueCode *>(uniqueCode)->unVendorId = unVendorId;
    reinterpret_cast<UniqueCode *>(uniqueCode)->unProductId = unProductId;
}

const char *GamepadNT::getName()
{
    return name.c_str();
}

const char *GamepadNT::getDisplayName()
{
    return displayName.c_str();
}

std::string GamepadNT::getInternalName()
{
    // Get the size of the buffer needed to store the device name
    UINT nameSize = 0;
    if (GetRawInputDeviceInfoW(hDevice, RIDI_DEVICENAME, NULL, &nameSize) != 0)
        return std::string("Generic Gamepad");

    // Allocate a buffer to hold the device name
    wchar_t *deviceName = new wchar_t[nameSize];

    // Retrieve the actual device name
    if (GetRawInputDeviceInfoW(hDevice, RIDI_DEVICENAME, deviceName, &nameSize) < 0)
    {
        delete[] deviceName;
        return std::string("Gamepad");
    }

    std::string out = convertWCharToString(deviceName);
    delete[] deviceName;
    return out;
}

std::string GamepadNT::getInternalDisplayName()
{
    // Get the device name (path) first
    unsigned int nameSize = 0;
    if (GetRawInputDeviceInfoW(hDevice, RIDI_DEVICENAME, NULL, &nameSize) != 0)
        return std::string("Generic Controller");

    wchar_t *deviceName = new wchar_t[nameSize];
    if (GetRawInputDeviceInfoW(hDevice, RIDI_DEVICENAME, deviceName, &nameSize) < 0)
    {
        delete[] deviceName;
        return std::string("Generic Controller");
    }

    // Get a device information set that contains devices matching the device name
    HDEVINFO deviceInfoSet = SetupDiGetClassDevsW(&GUID_DEVINTERFACE_HID, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (deviceInfoSet == INVALID_HANDLE_VALUE)
    {
        delete[] deviceName;
        return std::string("Generic Controller");
    }

    SP_DEVINFO_DATA deviceInfoData;
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    std::string out = std::string("Generic Controller");

    out = convertWCharToString(deviceName);

    // Enumerate devices and find the one matching our device path
    for (DWORD i = 0; SetupDiEnumDeviceInterfaces(deviceInfoSet, NULL, &GUID_DEVINTERFACE_HID, i, &deviceInterfaceData); i++)
    {
        // Get the required buffer size for the device interface detail data
        DWORD requiredSize = 0;
        SetupDiGetDeviceInterfaceDetailW(deviceInfoSet, &deviceInterfaceData, NULL, 0, &requiredSize, NULL);

        SP_DEVICE_INTERFACE_DETAIL_DATA_W *deviceDetailData = (SP_DEVICE_INTERFACE_DETAIL_DATA_W *)malloc(requiredSize);
        deviceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        if (SetupDiGetDeviceInterfaceDetailW(deviceInfoSet, &deviceInterfaceData, deviceDetailData, requiredSize, NULL, &deviceInfoData))
        {
            // Compare device path to find the matching one
            if (_wcsicmp((wchar_t *)deviceDetailData->DevicePath, deviceName) == 0)
            {
                out = convertWCharToString((wchar_t *)deviceDetailData->DevicePath);
                // Try to get the friendly name of the device
                wchar_t friendlyName[256];
                if (SetupDiGetDeviceRegistryPropertyW(deviceInfoSet, &deviceInfoData, SPDRP_FRIENDLYNAME, NULL,
                                                      (PBYTE)friendlyName, sizeof(friendlyName), NULL))
                {
                    out = convertWCharToString(friendlyName);
                }
                else if (SetupDiGetDeviceRegistryPropertyW(deviceInfoSet, &deviceInfoData, SPDRP_DEVICEDESC, NULL,
                                                           (PBYTE)friendlyName, sizeof(friendlyName), NULL))
                {
                    // Fallback to device description if friendly name is not available
                    out = convertWCharToString(friendlyName);
                }
                break;
            }
        }

        // Free the allocated memory for the device detail data
        free(deviceDetailData);
    }

    // Cleanup
    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    delete[] deviceName;
    return out;
}