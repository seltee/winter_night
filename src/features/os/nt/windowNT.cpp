#include "features/os/nt/windowNT.h"

#if defined(OS_WINDOWS)
#include "features/renderer/vulkan/rendererVulkanNT.h"
#include "features/sound/nt/soundSystemNT.h"
#include "features/logger/logger.h"
#include <windows.h>
#include <hidsdi.h>
#include <hidusage.h>
#include <hidpi.h>
#include <algorithm>
#include <shellscalingapi.h>
#include <string>

using namespace wne;

#define CLASS_NAME L"wne Window Class"

typedef BOOL(WINAPI *SetProcessDPIAwareFunc)();

LRESULT CALLBACK windowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

GamepadReport _parseHidReport(HANDLE hDevice, PHIDP_PREPARSED_DATA pPreparsedData, char *report, UINT reportLength);
std::wstring utf8ToWide(const std::string &utf8Str);

bool WindowNT::setup(int32 width, int32 height, WindowType type)
{
    setProcessDPIAware();

    uint displayWidth = GetSystemMetrics(SM_CXSCREEN);
    uint displayHeight = GetSystemMetrics(SM_CYSCREEN);
    if (type == WindowType::Borderless)
    {
        width = displayWidth;
        height = displayHeight;
    }

    this->width = width;
    this->height = height;
    this->windowType = type;

    HINSTANCE hInstance = GetModuleHandle(NULL);

    // Register the window class.
    WNDCLASSW wc = {};
    ZeroMemory(&wc, sizeof(WNDCLASSW));
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpfnWndProc = windowProcedure;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

    if (!RegisterClassW(&wc))
    {
        return false;
    }

    // Get the work area (area excluding the taskbar)
    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
    int visibleWidth = workArea.right - workArea.left;
    int visibleHeight = workArea.bottom - workArea.top;

    int positionX = type == WindowType::Windowed ? (visibleWidth - (width + getAdditionalWindowWidth())) / 2 : 0;
    int positionY = type == WindowType::Windowed ? (visibleHeight - (height + getAdditionalWindowHeight())) / 2 : 0;

    int style = getStyleForState(type);

    auto wideCaption = utf8ToWide(caption);

    // CreateWindowHandle
    hWnd = CreateWindowExW(
        0,                   // Optional window styles.
        CLASS_NAME,          // Window class
        wideCaption.c_str(), // Window text
        style,               // Window style
        // position and size
        positionX, positionY, width + getAdditionalWindowWidth(), height + getAdditionalWindowHeight(),
        NULL,      // Parent window
        NULL,      // Menu
        hInstance, // Instance handle
        this       // Additional application data
    );

    if (hWnd == NULL)
    {
        return false;
    }

    renderer = RendererVulkanNT::create(hWnd, width, height);
    if (!renderer)
    {
        CloseWindow(hWnd);
        return false;
    }

    Logger::log << "Window and Vulkan renderer succesfully created " << width << "x" << height << endl;

    soundSystem = SoundSystemNT::create(hWnd);
    if (!soundSystem)
    {
        CloseWindow(hWnd);
        return false;
    }

    Logger::log << "Windows NT sound system initializaed" << endl;

    ShowWindow(hWnd, true);
    UpdateWindow(hWnd);
    SetActiveWindow(hWnd);
    enableGamepadInput();

    prepareGamepadList();
    return true;
}

void WindowNT::update(float delta)
{
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (WM_QUIT == msg.message)
        {
            flagCloseRequested = true;
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    RECT rect;
    GetWindowRect((HWND__ *)hWnd, &rect);

    // 2026. Still better check than relying on Windows events
    // You can detect maximization of the window, but when you go backwards WM_RESTORE
    // is shared among other actions like changing the window size
    // The most reliable way is still to just check the window size manually
    int32 newWidth = rect.right - rect.left;
    int32 newHeight = rect.bottom - rect.top;
    if ((width != newWidth || height != newHeight) && IsWindow(hWnd))
    {
        width = newWidth;
        height = newHeight;
        renderer->changeWindowSize(width, height);
    }

    if (flagMouseCapture && flagFocused)
    {
        POINT pt = {(int)(width / 2), (int)(height / 2)};
        ClientToScreen(hWnd, &pt); // Converts client to screen coords
        SetCursorPos(pt.x, pt.y);  // Moves the mouse
        mousePositionX = width / 2;
        mousePositionY = height / 2;
    }

    renderer->update(delta);
    soundSystem->update();
}

void WindowNT::render()
{
    renderer->renderStart();
    renderer->renderScenes();
    renderer->renderDebug();
    renderer->renderFinish();
}

void WindowNT::updateWindowSize()
{
    RECT rect;
    GetWindowRect((HWND__ *)hWnd, &rect);
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;
    if (renderer)
    {
        renderer->changeWindowSize(width, height);
    }
}

void WindowNT::close()
{
    if (hWnd)
    {
        CloseWindow(hWnd);
        hWnd = nullptr;
    }
}

void WindowNT::setFocused(bool state)
{
    flagFocused = state;
    emitEventFocusChanged(state);
}

void WindowNT::handleDevicePlugged(void *handle)
{
    if (!findGamepad(handle))
    {
        gamepads.emplace_back(std::make_shared<GamepadNT>(handle));
        emitEventGamepadPlugged(gamepads.back());
    }
}

void WindowNT::handleDeviceUnplugged(void *handle)
{
    auto toRemove = findGamepad(handle);
    if (toRemove)
    {
        toRemove->setUnplugged();
        std::erase_if(gamepads, [&](const std::shared_ptr<wne::Gamepad> &gamepad)
                      { return gamepad.get() == toRemove.get(); });
        emitEventGamepadUnplugged(toRemove);
    }
}

std::shared_ptr<Gamepad> WindowNT::findGamepad(void *handle)
{
    for (auto &gamepad : gamepads)
    {
        if (static_cast<GamepadNT *>(gamepad.get())->getHandle() == handle)
            return gamepad;
    }
    return nullptr;
}

void WindowNT::prepareGamepadList()
{
    unsigned int deviceCount = 0;
    GetRawInputDeviceList(nullptr, &deviceCount, sizeof(RAWINPUTDEVICELIST));

    // Allocate memory for the device list
    std::vector<RAWINPUTDEVICELIST> deviceList;
    deviceList.resize(deviceCount);
    GetRawInputDeviceList(deviceList.data(), &deviceCount, sizeof(RAWINPUTDEVICELIST));

    // Iterate over all devices
    for (unsigned int i = 0; i < deviceCount; ++i)
    {
        // Check if the device is a gamepad (or joystick)
        if (deviceList[i].dwType == RIM_TYPEHID)
        {
            RID_DEVICE_INFO rdi;
            rdi.cbSize = sizeof(rdi);
            unsigned int rdiSize = sizeof(rdi);
            GetRawInputDeviceInfo(deviceList[i].hDevice, RIDI_DEVICEINFO, &rdi, &rdiSize);

            // Check for gamepad based on usage page and usage ID
            if (rdi.hid.usUsagePage == 0x01 && (rdi.hid.usUsage == 0x04 || rdi.hid.usUsage == 0x05))
            {
                // gamepads.emplace_back(std::make_shared<GamepadNT>(deviceList[i].hDevice));
                handleDevicePlugged(deviceList[i].hDevice);
            }
        }
    }
}

void WindowNT::setProcessDPIAware()
{
    HMODULE user32 = LoadLibraryA("user32.dll");
    if (user32 == NULL)
    {
        printf("Failed to load user32.dll");
        return;
    }

    // Get the address of SetProcessDPIAware
    SetProcessDPIAwareFunc SetProcessDPIAware = (SetProcessDPIAwareFunc)GetProcAddress(user32, "SetProcessDPIAware");

    if (SetProcessDPIAware)
        SetProcessDPIAware();

    // Free the library
    FreeLibrary(user32);
}

void WindowNT::enableGamepadInput()
{
    RAWINPUTDEVICE rid;
    rid.usUsagePage = 0x01;
    rid.usUsage = 0x05;
    rid.dwFlags = RIDEV_INPUTSINK | RIDEV_DEVNOTIFY;
    rid.hwndTarget = hWnd;

    if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == false)
        Logger::log << "Unable to register raw input devices for Windows" << endl;
}

int WindowNT::getStyleForState(WindowType type)
{
    switch (type)
    {
    case WindowType::Windowed:
        return WS_OVERLAPPEDWINDOW;
    case WindowType::Resizable:
        return WS_OVERLAPPEDWINDOW | WS_SIZEBOX | WS_VISIBLE;
    case WindowType::Fullscreen:
        return WS_POPUP | WS_VISIBLE;
    case WindowType::Borderless:
        return WS_POPUP | WS_VISIBLE;
    }
    return WS_OVERLAPPEDWINDOW;
}

float WindowNT::getAdditionalWindowWidth()
{
    int additionalWindowWidth = GetSystemMetrics(SM_CYFRAME) * 2 + 16;
    return static_cast<float>(additionalWindowWidth);
}

float WindowNT::getAdditionalWindowHeight()
{
    int additionalWindowHeight = GetSystemMetrics(SM_CYFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION) + 16;
    return static_cast<float>(additionalWindowHeight);
}

LRESULT CALLBACK windowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WindowNT *window = (WindowNT *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    int16 mousePositionX, mousePositionY;
    if (message == WM_CREATE)
    {
        CREATESTRUCT *CreateStruct = (CREATESTRUCT *)lParam;
        window = (WindowNT *)CreateStruct->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)window);
    }

    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return DefWindowProcW(hWnd, message, wParam, lParam);

    case WM_SETFOCUS:
        window->setFocused(true);
        break;

    case WM_KILLFOCUS:
        window->setFocused(false);
        break;

    case WM_KEYDOWN:
        window->emitEventKey(true, (lParam >> 16) & 0xFF);
        break;

    case WM_KEYUP:
        window->emitEventKey(false, (lParam >> 16) & 0xFF);
        break;

        // case WM_SETCURSOR:
        //{
        //  Check if the cursor is within the client area
        //  if (LOWORD(lParam) == HTCLIENT)
        //     window->resetCursorIcon();
        // break;
        //}

    case WM_MOUSEMOVE:
        mousePositionX = (int16)(lParam & 0xffff);
        mousePositionY = (int16)(lParam >> 16);
        if (window->mousePositionX != 0 && window->mousePositionY != 0)
        {
            window->emitEventMouseMove(
                (int16)(lParam & 0xffff) - window->mousePositionX,
                (int16)(lParam >> 16) - window->mousePositionY,
                mousePositionX,
                mousePositionY);
        }
        window->mousePositionX = mousePositionX;
        window->mousePositionY = mousePositionY;
        break;

    case WM_LBUTTONDOWN:
        window->emitEventMouseClick(true, MouseButton::LeftMouseButton);
        break;

    case WM_LBUTTONUP:
        window->emitEventMouseClick(false, MouseButton::LeftMouseButton);
        break;

    case WM_RBUTTONDOWN:
        window->emitEventMouseClick(true, MouseButton::RightMouseButton);
        break;

    case WM_RBUTTONUP:
        window->emitEventMouseClick(false, MouseButton::RightMouseButton);
        break;

    case WM_MBUTTONDOWN:
        window->emitEventMouseClick(true, MouseButton::MiddleMouseButton);
        break;

    case WM_MBUTTONUP:
        window->emitEventMouseClick(true, MouseButton::MiddleMouseButton);
        break;

    case WM_MOUSEWHEEL:
        break;

    case WM_WINDOWPOSCHANGED:
        break;

    case WM_MOUSEHOVER:
        // window->updateMouseOverWindow(true);
        break;

    case WM_MOUSELEAVE:
        // window->updateMouseOverWindow(false);
        break;

    case WM_INPUT:
    {
        UINT dwSize;
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));
        if (dwSize == 0)
            break;
        std::vector<uint8> lpb;
        lpb.resize(dwSize);

        auto rawInputDataSize = GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb.data(), &dwSize, sizeof(RAWINPUTHEADER));
        if (rawInputDataSize != dwSize)
        {
            std::cout << "GetRawInputData does not return correct size! " << rawInputDataSize << " vs " << dwSize << std::endl;
            break;
        }

        RAWINPUT *raw = (RAWINPUT *)lpb.data();
        if (raw->header.dwType == RIM_TYPEHID)
        {
            // Get preparsed data
            UINT preparsedDataSize;
            GetRawInputDeviceInfo(raw->header.hDevice, RIDI_PREPARSEDDATA, nullptr, &preparsedDataSize);
            std::vector<uint8> preparcedData;
            preparcedData.resize(preparsedDataSize);
            GetRawInputDeviceInfo(raw->header.hDevice, RIDI_PREPARSEDDATA, preparcedData.data(), &preparsedDataSize);

            // Gamepad data
            RID_DEVICE_INFO rdi;
            rdi.cbSize = sizeof(rdi);
            unsigned int rdiSize = sizeof(rdi);
            GetRawInputDeviceInfo(raw->header.hDevice, RIDI_DEVICEINFO, &rdi, &rdiSize);

            auto gamepad = window->findGamepad(raw->header.hDevice);
            if (gamepad)
            {
                static_cast<Gamepad *>(gamepad.get());

                GamepadReport report = _parseHidReport(
                    raw->header.hDevice,
                    (PHIDP_PREPARSED_DATA)preparcedData.data(),
                    (char *)raw->data.hid.bRawData,
                    raw->data.hid.dwSizeHid);

                auto buttonChanges = gamepad->processReportButtons(report);
                auto axisChanges = gamepad->processReportAxises(report);
                auto directionPadChanges = gamepad->processReportDirectionPad(report);

                for (const auto &change : buttonChanges)
                    window->emitEventGamepadButton(gamepad, change.keyId, change.isPressed);
                for (const auto &change : axisChanges)
                    window->emitEventGamepadAxes(gamepad, change.axisId, change.value);
                for (const auto &change : directionPadChanges)
                    window->emitEventGamepadDirectionPad(gamepad, change.value);
            }
        }
    }
    break;

    case WM_INPUT_DEVICE_CHANGE:
        if (wParam == GIDC_ARRIVAL)
        {
            window->handleDevicePlugged((HANDLE)lParam);
        }
        else if (wParam == GIDC_REMOVAL)
        {
            window->handleDeviceUnplugged((HANDLE)lParam);
        }
        break;

    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }

    return DefWindowProcW(hWnd, message, wParam, lParam);
}

GamepadReport _parseHidReport(HANDLE hDevice, PHIDP_PREPARSED_DATA pPreparsedData, char *report, UINT reportLength)
{
    GamepadReport gamepadReport;
    memset(&gamepadReport, 0, sizeof(GamepadReport));

    HIDP_CAPS caps;
    HidP_GetCaps(pPreparsedData, &caps);

    // Example: Handling Button Inputs
    unsigned short numButtonCaps = caps.NumberInputButtonCaps;
    HIDP_BUTTON_CAPS buttonCaps[numButtonCaps];
    HidP_GetButtonCaps(HidP_Input, buttonCaps, &numButtonCaps, pPreparsedData);

    unsigned long numUsages = Gamepad::MaxButtons; // Adjust based on expected number of buttons
    unsigned short usageList[Gamepad::MaxButtons];
    HidP_GetUsages(HidP_Input, buttonCaps[0].UsagePage, 0, usageList, &numUsages, pPreparsedData, report, reportLength);

    for (unsigned long i = 0; i < numUsages; i++)
    {
        gamepadReport.buttons[usageList[i]] = true;
    }

    // Example: Handling Axis Inputs
    unsigned short numValueCaps = caps.NumberInputValueCaps;
    HIDP_VALUE_CAPS valueCaps[numValueCaps];
    HidP_GetValueCaps(HidP_Input, valueCaps, &numValueCaps, pPreparsedData);

    for (USHORT i = 0; i < numValueCaps; i++)
    {
        unsigned short usage = valueCaps[i].NotRange.Usage;
        unsigned short page = valueCaps[i].UsagePage;

        unsigned long value;
        HidP_GetUsageValue(HidP_Input, page, 0, usage, &value, pPreparsedData, report, reportLength);
        float fValue = fminf(fmaxf((static_cast<float>(value) / static_cast<float>(0xffff / 2)) - 1.0f, -1.0f), 1.0f);

        if (usage >= 0 && usage < Gamepad::MaxAxises)
            gamepadReport.axises[usage] = fValue;

        // DPAD
        if (usage == 57)
            gamepadReport.directionPad = value;
    }

    return gamepadReport;
}

std::wstring utf8ToWide(const std::string &utf8Str)
{
    if (utf8Str.empty())
        return L"";

    int targetSize = MultiByteToWideChar(
        CP_UTF8,                            // Source code page (UTF-8)
        0,                                  // Flags (must be 0 for CP_UTF8)
        utf8Str.c_str(),                    // Pointer to source string
        static_cast<int>(utf8Str.length()), // Source string length in bytes
        nullptr,                            // Target buffer pointer (null to calculate size)
        0                                   // Target buffer size (0 to calculate size)
    );

    if (targetSize == 0)
    {
        throw std::system_error(
            GetLastError(),
            std::system_category(),
            "Failed to convert UTF-8 string to wide string");
    }

    std::wstring wideStr(targetSize, L'\0');

    MultiByteToWideChar(
        CP_UTF8,
        0,
        utf8Str.c_str(),
        static_cast<int>(utf8Str.length()),
        &wideStr[0], // Pointer to internal wstring buffer
        targetSize);

    return wideStr;
}

#endif