#include "features/os/nt/windowNT.h"
#include "features/renderer/vulkan/rendererVulkanNT.h"
#include "features/logger/logger.h"
#include <windows.h>

using namespace wne;

#define CLASS_NAME L"wne Window Class"

typedef BOOL(WINAPI *SetProcessDPIAwareFunc)();

LRESULT CALLBACK windowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

bool WindowNT::setup(uint width, uint height, WindowType type)
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

    // CreateWindowHandle
    hWnd = CreateWindowExW(
        0,               // Optional window styles.
        CLASS_NAME,      // Window class
        L"Window Title", // Window text
        style,           // Window style
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

    renderer = RendererVulkanNT::create(hWnd);
    if (!renderer)
    {
        CloseWindow(hWnd);
        return false;
    }

    Logger::log << "Window and Vulkan renderer succesfully created " << width << "x" << height << endl;

    ShowWindow(hWnd, true);
    UpdateWindow(hWnd);
    SetActiveWindow(hWnd);

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
    uint newWidth = rect.right - rect.left;
    uint newHeight = rect.bottom - rect.top;
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
}

void WindowNT::render()
{
    renderer->render();
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
        window->emitEventKey(true, wParam);
        break;

    case WM_KEYUP:
        window->emitEventKey(false, wParam);
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
            window->emitEventMouseMove((int16)(lParam & 0xffff) - window->mousePositionX, (int16)(lParam >> 16) - window->mousePositionY);
        }
        window->mousePositionX = mousePositionX;
        window->mousePositionY = mousePositionY;
        break;

    case WM_LBUTTONDOWN:
        window->emitEventMouseClick(true, 0);
        break;

    case WM_LBUTTONUP:
        window->emitEventMouseClick(false, 0);
        break;

    case WM_RBUTTONDOWN:
        window->emitEventMouseClick(true, 1);
        break;

    case WM_RBUTTONUP:
        window->emitEventMouseClick(false, 1);
        break;

    case WM_MBUTTONDOWN:
        window->emitEventMouseClick(true, 2);
        break;

    case WM_MBUTTONUP:
        window->emitEventMouseClick(true, 2);
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

    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }

    return DefWindowProcW(hWnd, message, wParam, lParam);
}
