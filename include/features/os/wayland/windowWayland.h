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
        bool setup(int32 width, int32 height, WindowType type) override final;

        void update(float delta) override final;
        void render() override final;
        void updateWindowSize() override final;
        void close() override final;

        void updateWindowSizeExt(int32 width, int32 height);

        wl_compositor *compositor_ = nullptr;
        xdg_wm_base *shell_ = nullptr;
        xdg_surface *shellSurface_ = nullptr;
        xdg_toplevel *toplevel_ = nullptr;

    private:
        wl_display *display_ = nullptr;
        wl_surface *surface_ = nullptr;

        static void handleToplevelConfigure(void *data,
                                            xdg_toplevel *toplevel,
                                            int32_t width,
                                            int32_t height,
                                            wl_array *states);
        static void handleToplevelClose(void *data, struct xdg_toplevel *toplevel);
        static void handleShellPing(void *data, struct xdg_wm_base *shell, uint32_t serial);
        static void handleShellSurfaceConfigure(void *data, struct xdg_surface *shellSurface, uint32_t serial);
        static void onRemoveRegistry(void *a, struct wl_registry *b, uint32_t c);
        static void onRegistry(void *data,
                               struct wl_registry *registry,
                               uint32_t name,
                               const char *interface,
                               uint32_t version);
    };
}
#endif