#include "features/os/nt/windowNT.h"
#include "features/renderer/vulkan/rendererVulkanNT.h"
#include <windows.h>
#include <iostream>

using namespace wne;

#define CLASS_NAME L"wne Window Class"

typedef BOOL(WINAPI *SetProcessDPIAwareFunc)();

LRESULT CALLBACK windowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

bool WindowNT::setup(int width, int height, WindowType type)
{
    setProcessDPIAware();

    int displayWidth = GetSystemMetrics(SM_CXSCREEN);
    int displayHeight = GetSystemMetrics(SM_CYSCREEN);
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

    renderer = RendererVulkanNT::create(hWnd, width, height);
    if (!renderer)
    {
        CloseWindow(hWnd);
        return false;
    }
    std::cout << "Window and renderer succesfully created" << std::endl;

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
            closeRequested = true;
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
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
    focused = state;
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
        break;

    case WM_SETFOCUS:
        window->setFocused(true);
        break;

    case WM_KILLFOCUS:
        window->setFocused(false);
        break;

    case WM_KEYDOWN:
        break;

    case WM_KEYUP:
        break;

        // case WM_SETCURSOR:
        //{
        //  Check if the cursor is within the client area
        //  if (LOWORD(lParam) == HTCLIENT)
        //     window->resetCursorIcon();
        // break;
        //}

    case WM_MOUSEMOVE:

        break;

    case WM_LBUTTONDOWN:

        break;

    case WM_LBUTTONUP:

        break;

    case WM_RBUTTONDOWN:

        break;

    case WM_RBUTTONUP:

        break;

    case WM_MBUTTONDOWN:

        break;

    case WM_MBUTTONUP:

        break;

    case WM_MOUSEWHEEL:

        break;

    case WM_WINDOWPOSCHANGED:
        break;

    case WM_EXITSIZEMOVE:
        window->updateWindowSize();
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

    return 0;
}
