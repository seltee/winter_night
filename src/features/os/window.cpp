#include "features/os/window.h"
#include "features/os/nt/windowNT.h"
#include "engine.h"

using namespace WNE;

Window::Window()
{
    Engine::getInstance()->registerWindow(this);
}

Window::~Window()
{
    Engine::getInstance()->unregisterWindow(this);
}

Window *Window::create(int width, int height)
{
    WindowNT *window = new WindowNT();
    if (window->setup(width, height, WindowType::Windowed))
    {
        return window;
    }
    delete window;
    return nullptr;
}

Window *Window::createFullscreen(int width, int height)
{
    WindowNT *window = new WindowNT();
    if (window->setup(width, height, WindowType::Fullscreen))
    {
        return window;
    }
    delete window;
    return nullptr;
}

Window *Window::createBorderless(int width, int height)
{
    WindowNT *window = new WindowNT();
    if (window->setup(width, height, WindowType::Borderless))
    {
        return window;
    }
    delete window;
    return nullptr;
}

Window *Window::createResizable(int width, int height)
{
    WindowNT *window = new WindowNT();
    if (window->setup(width, height, WindowType::Resizable))
    {
        return window;
    }
    delete window;
    return nullptr;
}

void Window::update(float delta)
{
}

void Window::render()
{
}

void Window::updateWindowSize()
{
}

void Window::close()
{
}

void Window::destroy()
{
    close();
    delete this;
}