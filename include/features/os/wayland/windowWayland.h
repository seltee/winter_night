#pragma once
#include "core/platform.h"

#if defined(OS_LINUX)
#include "features/os/window.h"
#include "core/api.h"
#include "core/core.h"
#include <memory>
#include <wayland-client.h>
#include "features/os/wayland/xdg-shell-client-protocol.h"

namespace wne
{
    class WNE_API WindowWayland : public Window
    {
    public:
        WindowWayland(const char *windowCaption) : Window(windowCaption) {}
        bool setup(int32 width, int32 height, WindowType type) override final;

        void update(float delta) override final;
        void render() override final;
        void updateWindowSize() override final;
        void close() override final;

        void updateWindowSizeExt(int32 width, int32 height);
        void setScaleFactor(int32 scaleFactor);

        void subscribePointer(wl_pointer *pointer);
        void subscribeKeyboard(wl_keyboard *keyboard);

        void provideMousePosition(float x, float y);
        void provideMouseButton(MouseButton button, bool state);
        void provideKeyboardKey(uint16 key, bool state);

        wl_compositor *compositor_ = nullptr;
        xdg_wm_base *shell_ = nullptr;
        xdg_surface *shellSurface_ = nullptr;
        wl_output *output_ = nullptr;
        wl_seat *seat_ = nullptr;
        xdg_toplevel *toplevel_ = nullptr;

    private:
        wl_display *display_ = nullptr;
        wl_surface *surface_ = nullptr;
        int32 scaleFactor = 1;
        int32 mouseX = 0;
        int32 mouseY = 0;
        bool flagShowTitlebar = false;

        std::shared_ptr<Scene> uiScene;

        std::shared_ptr<Scene> createUIScene();

        static void handleToplevelConfigure(void *data,
                                            xdg_toplevel *toplevel,
                                            int32_t width,
                                            int32_t height,
                                            wl_array *states);
        static void handleToplevelClose(void *data, xdg_toplevel *toplevel);
        static void handleShellPing(void *data, xdg_wm_base *shell, uint32_t serial);
        static void handleShellSurfaceConfigure(void *data, xdg_surface *shellSurface, uint32_t serial);
        static void handleOutputGeometry(void *, wl_output *,
                                         int32, int32,
                                         int32, int32,
                                         int32,
                                         const char *,
                                         const char *,
                                         int32);
        static void handleOutputMode(void *data,
                                     wl_output *output,
                                     uint32 flags,
                                     int32 width,
                                     int32 height,
                                     int32 refresh);
        static void handleOutputDone(void *, wl_output *);
        static void handleOutputScale(void *data,
                                      wl_output *output,
                                      int32 factor);

        static void handleSeatCapabilities(
            void *data,
            wl_seat *seat,
            uint32 capabilities);

        static void handleSeatName(
            void *data,
            wl_seat *seat,
            const char *name);

        static void handlePointerEnter(void *data,
                                       wl_pointer *wl_pointer,
                                       uint32 serial,
                                       wl_surface *surface,
                                       wl_fixed_t surfaceX,
                                       wl_fixed_t surfaceY);
        static void handlePointerLeave(void *data,
                                       wl_pointer *wl_pointer,
                                       uint32 serial,
                                       wl_surface *surface);
        static void handlePointerMotion(void *data,
                                        wl_pointer *wl_pointer,
                                        uint32 time,
                                        wl_fixed_t surfaceX,
                                        wl_fixed_t surfaceY);
        static void handlePointerButton(void *data,
                                        wl_pointer *wl_pointer,
                                        uint32 serial,
                                        uint32 time,
                                        uint32 button,
                                        uint32 state);
        static void handlePointerAxis(void *data,
                                      wl_pointer *wl_pointer,
                                      uint32 time,
                                      uint32 axis,
                                      wl_fixed_t value);

        static void hanldeKeyboardKeyMap(void *data,
                                         wl_keyboard *wl_keyboard,
                                         uint32 format,
                                         int32 fd,
                                         uint32 size);
        static void handleKeyboardEnter(void *data,
                                        wl_keyboard *wl_keyboard,
                                        uint32 serial,
                                        wl_surface *surface,
                                        wl_array *keys);
        static void handleKeyboardLeave(void *data,
                                        struct wl_keyboard *wl_keyboard,
                                        uint32 serial,
                                        struct wl_surface *surface);
        static void handleKeyboardKey(void *data,
                                      wl_keyboard *wl_keyboard,
                                      uint32 serial,
                                      uint32 time,
                                      uint32 key,
                                      uint32 state);
        static void handleKeyboardModifiers(void *data,
                                            struct wl_keyboard *wl_keyboard,
                                            uint32 serial,
                                            uint32 mods_depressed,
                                            uint32 mods_latched,
                                            uint32 mods_locked,
                                            uint32 group);
        static void handleKeyboardRepeatInfo(void *data,
                                             struct wl_keyboard *wl_keyboard,
                                             int32 rate,
                                             int32 delay);

        static void onRemoveRegistry(void *a, wl_registry *b, uint32_t c);
        static void onRegistry(void *data,
                               wl_registry *registry,
                               uint32 name,
                               const char *interface,
                               uint32 version);
    };
}
#endif