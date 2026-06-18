#include "features/os/wayland/windowWayland.h"

#if defined(OS_LINUX)
#include "features/renderer/vulkan/rendererVulkanWayland.h"
#include "features/logger/logger.h"
#include <algorithm>
#include <string>

using namespace wne;

bool WindowWayland::setup(uint width, uint height, WindowType type)
{
    display = wl_display_connect(nullptr);
    if (!display)
    {
        Logger::log << "Wayland is unable to open Window";
        return false;
    }

    wl_registry *registry = wl_display_get_registry(display);
    const struct wl_registry_listener registry_listener = {
        WindowWayland::onRegistry, WindowWayland::onRemoveRegistry};
    wl_registry_add_listener(registry, &registry_listener, this);
    wl_display_roundtrip(display);
    wl_registry_destroy(registry);

    if (!compositor_)
        return false;

    surface_ = wl_compositor_create_surface(compositor_);
    if (!surface_)
      return false;

    renderer = RendererVulkanWayland::create(display, surface_);
    if (!renderer)
        return false;

    return true;
}

void WindowWayland::update(float delta)
{
}

void WindowWayland::render()
{
    renderer->render();
}

void WindowWayland::updateWindowSize()
{
    if (renderer)
    {
        // renderer->changeWindowSize(width, height);
    }
}

void WindowWayland::close()
{
}

void WindowWayland::onRemoveRegistry(void *a, struct wl_registry *b, uint32_t c)
{
}

void WindowWayland::onRegistry(void *data,
                               struct wl_registry *registry,
                               uint32_t name,
                               const char *interface,
                               uint32_t version)
{
    WindowWayland *me = static_cast<WindowWayland *>(data);
    if (!strcmp(interface, wl_compositor_interface.name))
    {
        me->compositor_ = static_cast<wl_compositor *>(
            wl_registry_bind(registry, name, &wl_compositor_interface, version));
    }
    else if (!strcmp(interface, wl_shm_interface.name))
    {
        me->shm_ = static_cast<wl_shm *>(
            wl_registry_bind(registry, name, &wl_shm_interface, version));
    }
    else if (!strcmp(interface, wl_shell_interface.name))
    {
        me->shell_ = static_cast<wl_shell *>(
            wl_registry_bind(registry, name, &wl_shell_interface, version));
    }
    /*
    else if (!strcmp(interface, wl_seat_interface.name))
    {
        me->seat_ = static_cast<wl_seat *>(
            wl_registry_bind(registry, name, &wl_seat_interface, version));
        me->pointer_ = wl_seat_get_pointer(me->seat_);
        const struct wl_pointer_listener pointer_listener = {
            OnPointerEnter, OnPointerLeave, OnPointerMotion, OnPointerButton,
            OnPointerAxis};
        wl_pointer_add_listener(me->pointer_, &pointer_listener, data);
    }
    */
}
#endif