#pragma once
#include "core/platform.h"

#if defined(OS_LINUX)
#include "features/os/window.h"
#include "core/api.h"
#include <memory>
#include <wayland-client.h>

namespace wne
{
    class WNE_API WindowWayland : public Window
    {
    public:
        bool setup(uint width, uint height, WindowType type);

        void update(float delta) override final;
        void render() override final;
        void updateWindowSize() override final;
        void close() override final;

        wl_compositor *compositor_ = nullptr;
        wl_pointer *pointer_ = nullptr;
        wl_seat *seat_ = nullptr;
        wl_shell *shell_ = nullptr;
        wl_shm *shm_ = nullptr;

    private:
        wl_display *display = nullptr;
        wl_surface *surface_ = nullptr;

        static void onRemoveRegistry(void *a, struct wl_registry *b, uint32_t c);
        static void onRegistry(void *data,
                               struct wl_registry *registry,
                               uint32_t name,
                               const char *interface,
                               uint32_t version);
    };
}
#endif