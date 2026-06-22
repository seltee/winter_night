#include "features/os/wayland/windowWayland.h"

#if defined(OS_LINUX)
#include "features/renderer/vulkan/rendererVulkanWayland.h"
#include "features/logger/logger.h"
#include <algorithm>
#include <string>

using namespace wne;

bool WindowWayland::setup(int32 width, int32 height, WindowType type)
{
    display_ = wl_display_connect(NULL);
    if (!display_)
    {
        Logger::log << "Wayland is unable to open Window";
        return false;
    }

    wl_registry *registry = wl_display_get_registry(display_);
    const struct wl_registry_listener registry_listener = {
        WindowWayland::onRegistry, WindowWayland::onRemoveRegistry};
    wl_registry_add_listener(registry, &registry_listener, this);
    wl_display_roundtrip(display_);
    if (!compositor_)
    {
        Logger::log << "Wayland compositor is not set" << endl;
        return false;
    }

    surface_ = wl_compositor_create_surface(compositor_);
    if (!surface_)
    {
        Logger::log << "Unable to create surface" << endl;
        return false;
    }

    static const wl_output_listener outputListener = {
        .geometry = handleOutputGeometry,
        .mode = handleOutputMode,
        .done = handleOutputDone,
        .scale = handleOutputScale,
    };
    wl_output_add_listener(output_, &outputListener, this);

    wl_surface_commit(surface_);
    wl_display_roundtrip(display_);
    wl_surface_commit(surface_);

    static const xdg_surface_listener shellSurfaceListener = {
        .configure = WindowWayland::handleShellSurfaceConfigure};

    shellSurface_ = xdg_wm_base_get_xdg_surface(shell_, surface_);
    xdg_surface_add_listener(shellSurface_, &shellSurfaceListener, this);

    static const xdg_toplevel_listener toplevelListener = {
        .configure = handleToplevelConfigure,
        .close = handleToplevelClose};

    toplevel_ = xdg_surface_get_toplevel(shellSurface_);
    xdg_toplevel_add_listener(toplevel_, &toplevelListener, this);

    xdg_toplevel_set_title(toplevel_, "Wow");
    xdg_toplevel_set_app_id(toplevel_, "Wow");

    wl_surface_commit(surface_);
    wl_display_roundtrip(display_);
    wl_surface_commit(surface_);

    for (int i = 0; i < 10; ++i)
    {
        wl_display_dispatch_pending(display_);
        wl_display_roundtrip(display_);
    }

    renderer = RendererVulkanWayland::create(display_, surface_, width, height);
    if (!renderer)
    {
        Logger::log << "Unable to create renderer" << endl;
        return false;
    }

    soundSystem = SoundSystem::create();
    if (!soundSystem)
    {
        Logger::log << "Unable to create sound system" << endl;
        return false;
    }

    return true;
}

void WindowWayland::update(float delta)
{
    wl_display_roundtrip(display_);
}

void WindowWayland::render()
{
    renderer->render();
}

void WindowWayland::updateWindowSize()
{
}

void WindowWayland::close()
{
    flagCloseRequested = true;
}

void WindowWayland::updateWindowSizeExt(int32 width, int32 height)
{
    this->width = width * scaleFactor;
    this->height = height * scaleFactor;
    if (renderer)
    {
        renderer->changeWindowSize(this->width, this->height);
    }
}

void WindowWayland::setScaleFactor(int32 scaleFactor)
{
    this->scaleFactor = scaleFactor;
    wl_surface_set_buffer_scale(surface_, scaleFactor);
}

void WindowWayland::handleToplevelConfigure(
    void *data,
    xdg_toplevel *toplevel,
    int32 width,
    int32 height,
    wl_array *states)
{
    WindowWayland *window = static_cast<WindowWayland *>(data);
    if (width != 0 && height != 0)
    {
        window->updateWindowSizeExt(width, height);
    }
}

void WindowWayland::handleToplevelClose(void *data, struct xdg_toplevel *toplevel)
{
    WindowWayland *window = static_cast<WindowWayland *>(data);
    window->close();
}

void WindowWayland::handleShellPing(void *data, struct xdg_wm_base *shell, uint32_t serial)
{
    xdg_wm_base_pong(shell, serial);
}

void WindowWayland::handleShellSurfaceConfigure(void *data, struct xdg_surface *shellSurface, uint32_t serial)
{
    xdg_surface_ack_configure(shellSurface, serial);
}

void WindowWayland::handleOutputGeometry(void *, wl_output *,
                                         int32, int32,
                                         int32, int32,
                                         int32,
                                         const char *,
                                         const char *,
                                         int32)
{
}

void WindowWayland::handleOutputMode(void *data,
                                     wl_output *output,
                                     uint32 flags,
                                     int32 width,
                                     int32 height,
                                     int32 refresh)
{
    if (flags & WL_OUTPUT_MODE_CURRENT)
    {
        Logger::log << "Current display mode " << width << "x" << height << endl;
    }
}

void WindowWayland::handleOutputDone(void *, wl_output *)
{
}

void WindowWayland::handleOutputScale(void *data,
                                      wl_output *output,
                                      int32 factor)
{
    WindowWayland *window = static_cast<WindowWayland *>(data);
    window->setScaleFactor(factor);
}

void WindowWayland::onRemoveRegistry(void *a, struct wl_registry *b, uint32_t c)
{
}

void WindowWayland::onRegistry(void *data,
                               struct wl_registry *registry,
                               uint32 name,
                               const char *interface,
                               uint32 version)
{
    WindowWayland *window = static_cast<WindowWayland *>(data);
    if (!strcmp(interface, wl_compositor_interface.name))
    {
        window->compositor_ = static_cast<wl_compositor *>(
            wl_registry_bind(registry, name, &wl_compositor_interface, version));
    }
    else if (!strcmp(interface, "xdg_wm_base"))
    {
        static const xdg_wm_base_listener shellListener = {
            .ping = WindowWayland::handleShellPing};

        window->shell_ = static_cast<xdg_wm_base *>(wl_registry_bind(registry, name, &xdg_wm_base_interface, 1));
        xdg_wm_base_add_listener(window->shell_, &shellListener, NULL);
    }
    else if (!strcmp(interface, wl_output_interface.name))
    {
        window->output_ = static_cast<wl_output *>(
            wl_registry_bind(
                registry,
                name,
                &wl_output_interface,
                2));
    }
}

#endif
