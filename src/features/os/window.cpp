#include "features/os/window.h"
#include "features/os/nt/windowNT.h"
#include "engine.h"

using namespace wne;

Window::Window()
{
    Engine::getInstance()->registerWindow(this);
}

Window::~Window()
{
    close();
    Engine::getInstance()->unregisterWindow(this);
}

std::unique_ptr<Window> Window::create(int width, int height)
{
    std::unique_ptr<WindowNT> window = std::make_unique<WindowNT>();
    if (window->setup(width, height, WindowType::Windowed))
        return window;
    return nullptr;
}

std::unique_ptr<Window> Window::createFullscreen(int width, int height)
{
    std::unique_ptr<WindowNT> window = std::make_unique<WindowNT>();
    if (window->setup(width, height, WindowType::Fullscreen))
        return window;
    return nullptr;
}

std::unique_ptr<Window> Window::createBorderless(int width, int height)
{
    std::unique_ptr<WindowNT> window = std::make_unique<WindowNT>();
    if (window->setup(width, height, WindowType::Borderless))
        return window;
    return nullptr;
}

std::unique_ptr<Window> Window::createResizable(int width, int height)
{
    std::unique_ptr<WindowNT> window = std::make_unique<WindowNT>();
    if (window->setup(width, height, WindowType::Resizable))
        return window;
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