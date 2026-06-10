#pragma once
#include "core/api.h"
#include "features/renderer/renderer.h"
#include "features/os/windowEvents.h"
#include "features/os/gamepad.h"
#include <memory>
#include <vector>
#include <mutex>

namespace wne
{
    enum class WindowType
    {
        Windowed,
        Fullscreen,
        Borderless,
        Resizable
    };

    class WNE_API Window
    {
    protected:
        const static int MAX_SUBSCIRBERS = 100;

        uint width, height;
        WindowType windowType;
        bool flagFocused = false;
        bool flagCloseRequested = false;
        bool flagMouseCapture = false;
        std::unique_ptr<Renderer> renderer;
        std::mutex mutex;

        std::vector<std::shared_ptr<Gamepad>> gamepads;

        std::weak_ptr<WindowEvents> subscribers[MAX_SUBSCIRBERS];
        int subscribersAmount = 0;

        Window();

    public:
        virtual ~Window();

        static std::shared_ptr<Window> create(uint width, uint height);
        static std::shared_ptr<Window> createFullscreen(uint width, uint height);
        static std::shared_ptr<Window> createBorderless(uint width, uint height);
        static std::shared_ptr<Window> createResizable(uint width, uint height);

        virtual void update(float delta);
        virtual void render();
        virtual void updateWindowSize();

        virtual void close();

        std::shared_ptr<WindowEvents> subscribe();
        void emitEventKey(bool isPressed, uint16 keyCode);
        void emitEventMouseMove(int16 shiftX, int16 shiftY, int16 positionX, int16 positionY);
        void emitEventMouseClick(bool isPressed, uint16 mouseButton);
        void emitEventFocusChanged(bool newFocusState);
        void emitEventGamepadPlugged(std::shared_ptr<Gamepad> gamepad);
        void emitEventGamepadUnplugged(std::shared_ptr<Gamepad> gamepad);
        void emitEventGamepadButton(std::shared_ptr<Gamepad> gamepad, uint16 buttonCode, bool isPressed);
        void emitEventGamepadAxes(std::shared_ptr<Gamepad> gamepad, uint16 axisCode, float axisValue);
        void emitEventGamepadDirectionPad(std::shared_ptr<Gamepad> gamepad, uint16 value);

        inline Renderer *getRenderer()
        {
            return renderer.get();
        }

        inline uint getWidth()
        {
            return width;
        }

        inline uint getHeight()
        {
            return height;
        }

        inline WindowType getType()
        {
            return windowType;
        }

        inline bool isFocused()
        {
            return flagFocused;
        }

        inline bool isCloseRequested()
        {
            return flagCloseRequested;
        }

        inline void setMouseCapture(bool state)
        {
            flagMouseCapture = state;
        }

        inline bool idMouseCaptured()
        {
            return flagMouseCapture;
        }
    };
};