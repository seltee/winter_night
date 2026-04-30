#pragma once
#include "core/api.h"
#include "features/renderer/renderer.h"

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
        Renderer *renderer = nullptr;

        Window();
        virtual ~Window();

    public:
        static Window *create(int width, int height);
        static Window *createFullscreen(int width, int height);
        static Window *createBorderless(int width, int height);
        static Window *createResizable(int width, int height);

        virtual void update(float delta);
        virtual void render();
        virtual void updateWindowSize();

        virtual void close();
        void destroy();

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