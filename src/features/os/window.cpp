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

void Window::emitEventKey(bool isPressed, uint16 keyCode)
{
    std::lock_guard<std::mutex> lock(mutex);
    for (int i = 0; i < subscribersAmount;)
    {
        auto subscriber = subscribers[i].lock();
        if (subscriber)
        {
            subscriber->pushEventKey(isPressed, keyCode);
            i++;
        }
        else
        {
            subscribersAmount--;
            subscribers[i] = subscribers[subscribersAmount];
        }
    }
}

void Window::emitEventMouseMove(int16 shiftX, int16 shiftY, int16 positionX, int16 positionY)
{
    std::lock_guard<std::mutex> lock(mutex);
    for (int i = 0; i < subscribersAmount;)
    {
        auto subscriber = subscribers[i].lock();
        if (subscriber)
        {
            subscriber->pushEventMouseMove(shiftX, shiftY, positionX, positionY);
            i++;
        }
        else
        {
            subscribersAmount--;
            subscribers[i] = subscribers[subscribersAmount];
        }
    }
}

void Window::emitEventMouseClick(bool isPressed, uint16 mouseButton)
{
    std::lock_guard<std::mutex> lock(mutex);
    for (int i = 0; i < subscribersAmount;)
    {
        auto subscriber = subscribers[i].lock();
        if (subscriber)
        {
            subscriber->pushEventMouseClick(isPressed, mouseButton);
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

void Window::emitEventGamepadPlugged(std::shared_ptr<Gamepad> gamepad)
{
    std::lock_guard<std::mutex> lock(mutex);
    for (int i = 0; i < subscribersAmount;)
    {
        auto subscriber = subscribers[i].lock();
        if (subscriber)
        {
            subscriber->pushEventGamepadPlugged(gamepad);
            i++;
        }
        else
        {
            subscribersAmount--;
            subscribers[i] = subscribers[subscribersAmount];
        }
    }
}

void Window::emitEventGamepadUnplugged(std::shared_ptr<Gamepad> gamepad)
{
    std::lock_guard<std::mutex> lock(mutex);
    for (int i = 0; i < subscribersAmount;)
    {
        auto subscriber = subscribers[i].lock();
        if (subscriber)
        {
            subscriber->pushEventGamepadUnplugged(gamepad);
            i++;
        }
        else
        {
            subscribersAmount--;
            subscribers[i] = subscribers[subscribersAmount];
        }
    }
}

void Window::emitEventGamepadButton(std::shared_ptr<Gamepad> gamepad, uint16 buttonCode, bool isPressed)
{
    std::lock_guard<std::mutex> lock(mutex);
    for (int i = 0; i < subscribersAmount;)
    {
        auto subscriber = subscribers[i].lock();
        if (subscriber)
        {
            subscriber->pushEventGamepadButton(gamepad, buttonCode, isPressed);
            i++;
        }
        else
        {
            subscribersAmount--;
            subscribers[i] = subscribers[subscribersAmount];
        }
    }
}

void Window::emitEventGamepadAxes(std::shared_ptr<Gamepad> gamepad, uint16 axisCode, float axisValue)
{
    std::lock_guard<std::mutex> lock(mutex);
    for (int i = 0; i < subscribersAmount;)
    {
        auto subscriber = subscribers[i].lock();
        if (subscriber)
        {
            subscriber->pushEventGamepadAxis(gamepad, axisCode, axisValue);
            i++;
        }
        else
        {
            subscribersAmount--;
            subscribers[i] = subscribers[subscribersAmount];
        }
    }
}

void Window::emitEventGamepadDirectionPad(std::shared_ptr<Gamepad> gamepad, uint16 value)
{
    std::lock_guard<std::mutex> lock(mutex);
    for (int i = 0; i < subscribersAmount;)
    {
        auto subscriber = subscribers[i].lock();
        if (subscriber)
        {
            subscriber->pushEventGamepadDirectionPad(gamepad, value);
            i++;
        }
        else
        {
            subscribersAmount--;
            subscribers[i] = subscribers[subscribersAmount];
        }
    }
}