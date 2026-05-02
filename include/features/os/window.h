#pragma once
#include "core/api.h"
#include "features/renderer/renderer.h"
#include <memory>

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
        uint width, height;
        WindowType windowType;
        bool focused = false;
        bool closeRequested = false;
        std::unique_ptr<Renderer> renderer;

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
            return focused;
        }

        inline bool isCloseRequested()
        {
            return closeRequested;
        }
    };

}