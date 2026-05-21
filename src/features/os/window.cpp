#include "features/os/window.h"
#include "features/os/nt/windowNT.h"
#include "engine.h"
#include <iostream>

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

std::shared_ptr<Window> Window::create(uint width, uint height)
{
    std::shared_ptr<WindowNT> window = std::make_shared<WindowNT>();
    if (window->setup(width, height, WindowType::Windowed))
        return window;
    return nullptr;
}

std::shared_ptr<Window> Window::createFullscreen(uint width, uint height)
{
    std::shared_ptr<WindowNT> window = std::make_shared<WindowNT>();
    if (window->setup(width, height, WindowType::Fullscreen))
        return window;
    return nullptr;
}

std::shared_ptr<Window> Window::createBorderless(uint width, uint height)
{
    std::shared_ptr<WindowNT> window = std::make_shared<WindowNT>();
    if (window->setup(width, height, WindowType::Borderless))
        return window;
    return nullptr;
}

std::shared_ptr<Window> Window::createResizable(uint width, uint height)
{
    std::shared_ptr<WindowNT> window = std::make_shared<WindowNT>();
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

std::shared_ptr<WindowEvents> Window::subscribe()
{
    std::lock_guard<std::mutex> lock(mutex);
    if (subscribersAmount >= MAX_SUBSCIRBERS)
        return nullptr;

    std::shared_ptr sub = std::make_shared<WindowEvents>();
    subscribers[subscribersAmount] = sub;
    subscribersAmount++;
    return sub;
}

void Window::emitEventKey(bool isDown, uint16 keyCode)
{
    std::lock_guard<std::mutex> lock(mutex);
    for (int i = 0; i < subscribersAmount;)
    {
        auto subscriber = subscribers[i].lock();
        if (subscriber)
        {
            subscriber->pushEventKey(isDown, keyCode);
            i++;
        }
        else
        {
            subscribersAmount--;
            subscribers[i] = subscribers[subscribersAmount];
        }
    }
}

void Window::emitEventMouseMove(int16 shiftX, int16 shiftY)
{
    std::lock_guard<std::mutex> lock(mutex);
    for (int i = 0; i < subscribersAmount;)
    {
        auto subscriber = subscribers[i].lock();
        if (subscriber)
        {
            subscriber->pushEventMouseMove(shiftX, shiftY);
            i++;
        }
        else
        {
            subscribersAmount--;
            subscribers[i] = subscribers[subscribersAmount];
        }
    }
}

void Window::emitEventMouseClick(bool isDown, uint16 mouseButton)
{
    std::lock_guard<std::mutex> lock(mutex);
    for (int i = 0; i < subscribersAmount;)
    {
        auto subscriber = subscribers[i].lock();
        if (subscriber)
        {
            subscriber->pushEventMouseClick(isDown, mouseButton);
            i++;
        }
        else
        {
            subscribersAmount--;
            subscribers[i] = subscribers[subscribersAmount];
        }
    }
}

void Window::emitEventFocusChanged(bool newFocusState)
{
    std::lock_guard<std::mutex> lock(mutex);
    for (int i = 0; i < subscribersAmount;)
    {
        auto subscriber = subscribers[i].lock();
        if (subscriber)
        {
            subscriber->pushEventWindowFocusChanged(newFocusState);
            i++;
        }
        else
        {
            subscribersAmount--;
            subscribers[i] = subscribers[subscribersAmount];
        }
    }
}