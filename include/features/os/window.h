#pragma once
#include "core/api.h"
#include "core/platform.h"
#include "features/renderer/renderer.h"
#include "features/sound/soundSystem.h"
#include "features/os/windowEvents.h"
#include "features/os/gamepad.h"
#include "features/os/windowDefines.h"
#include "features/os/windowKeys.h"
#include <memory>
#include <vector>
#include <mutex>
#include <string>

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

        int32 width, height;
        WindowType windowType;
        bool flagFocused = false;
        bool flagCloseRequested = false;
        bool flagMouseCapture = false;
        std::unique_ptr<Renderer> renderer;
        std::shared_ptr<SoundSystem> soundSystem;
        std::mutex mutex;
        std::string caption = "Window Caption";

        std::vector<std::shared_ptr<Gamepad>> gamepads;

        std::weak_ptr<WindowEvents> subscribers[MAX_SUBSCIRBERS];
        int subscribersAmount = 0;

        Window(const char *windowCaption);
        static std::shared_ptr<Window> createWindow(const char *windowCaption);

    public:
        virtual ~Window();

        static std::shared_ptr<Window> create(const char *windowCaption, int32 width, int32 height);
        static std::shared_ptr<Window> createFullscreen(const char *windowCaption, int32 width, int32 height);
        static std::shared_ptr<Window> createBorderless(const char *windowCaption, int32 width, int32 height);
        static std::shared_ptr<Window> createResizable(const char *windowCaption, int32 width, int32 height);

        virtual bool setup(int32 width, int32 height, WindowType type);
        virtual void update(float delta);
        virtual void render();
        virtual void updateWindowSize();

        virtual void close();

        std::shared_ptr<WindowEvents> subscribe();
        void emitEventKey(bool isPressed, uint16 keyCode);
        void emitEventMouseMove(int16 shiftX, int16 shiftY, int16 positionX, int16 positionY);
        void emitEventMouseClick(bool isPressed, MouseButton mouseButton);
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

        inline SoundSystem *getSoundSystem()
        {
            return soundSystem.get();
        }

        inline int32 getWidth()
        {
            return width;
        }

        inline int32 getHeight()
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