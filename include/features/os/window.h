#pragma once
#include "core/api.h"
#include "features/renderer/renderer.h"
#include <memory>

namespace WNE
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
        int width, height;
        WindowType windowType;
        bool focused = false;
        bool closeRequested = false;
        std::unique_ptr<Renderer> renderer;

        Window();

    public:
        virtual ~Window();

        static std::unique_ptr<Window> create(int width, int height);
        static std::unique_ptr<Window> createFullscreen(int width, int height);
        static std::unique_ptr<Window> createBorderless(int width, int height);
        static std::unique_ptr<Window> createResizable(int width, int height);

        virtual void update(float delta);
        virtual void render();
        virtual void updateWindowSize();

        virtual void close();

        inline Renderer *getRenderer()
        {
            return renderer.get();
        }

        inline int getWidth()
        {
            return width;
        }

        inline int getHeight()
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