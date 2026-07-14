#include "features/os/wayland/windowWayland.h"

#if defined(OS_LINUX)
#include "features/renderer/vulkan/rendererVulkanWayland.h"
#include "features/scene/actorCamera.h"
#include "features/camera/cameraOrtho.h"
#include "features/scene/actorUI.h"
#include "features/logger/logger.h"
#include "features/sound/unix/soundSystemUnix.h"
#include <algorithm>
#include <string>

using namespace wne;

bool WindowWayland::setup(int32 width, int32 height, WindowType type)
{
    if (type == WindowType::Resizable || type == WindowType::Windowed)
        flagShowTitlebar = true;

    display_ = wl_display_connect(NULL);
    if (!display_)
    {
        Logger::log << "Wayland is unable to open Window";
        return false;
    }

    wl_registry *registry = wl_display_get_registry(display_);
    const wl_registry_listener registry_listener = {
        WindowWayland::onRegistry, WindowWayland::onRemoveRegistry};
    wl_registry_add_listener(registry, &registry_listener, this);
    wl_display_roundtrip(display_);
    if (!compositor_)
    {
        Logger::log << "Wayland compositor is not set" << endl;
        return false;
    }

    cursorSurface_ = wl_compositor_create_surface(compositor_);

    static const wl_seat_listener seatListener = {
        .capabilities = handleSeatCapabilities,
        .name = handleSeatName};
    wl_seat_add_listener(seat_, &seatListener, this);

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

    xdg_toplevel_set_title(toplevel_, caption.c_str());
    xdg_toplevel_set_app_id(toplevel_, caption.c_str());

    if (type == WindowType::Fullscreen)
    {
        xdg_toplevel_set_fullscreen(toplevel_, nullptr);
    }

    wl_surface_commit(surface_);
    wl_display_roundtrip(display_);
    wl_surface_commit(surface_);

    for (int i = 0; i < 10; ++i)
    {
        wl_display_dispatch_pending(display_);
        wl_display_roundtrip(display_);
    }

    gamepadSystemUnix = std::make_unique<GamepadSystemUnix>(this);
    gamepadSystemUnix->setup();

    renderer = RendererVulkanWayland::create(display_, surface_, width, height);
    if (!renderer)
    {
        Logger::log << "Unable to create renderer" << endl;
        return false;
    }

    soundSystem = SoundSystemUnix::create();
    if (!soundSystem)
    {
        Logger::log << "Unable to create sound system" << endl;
        return false;
    }

    return true;
}

void WindowWayland::update(float delta)
{
    gamepadSystemUnix->update();
    wl_display_dispatch_pending(display_);
    wl_display_flush(display_);
    renderer->update(delta);
    if (flagShowTitlebar)
    {
        if (!uiSceneData.actor)
            uiSceneData = createWaylandUIScene(renderer.get(), this);
        if (!uiSceneData.caption->isText(caption.c_str()))
            uiSceneData.caption->setText(caption.c_str());
        uiSceneData.actor->setDimensions(width, height);
        uiSceneData.camera->setSize(width, height);
        uiSceneData.scene->update(delta);
    }
    soundSystem->update();
}

void WindowWayland::render()
{
    renderer->renderStart();
    renderer->renderScenes();
    renderer->renderDebug();
    if (flagShowTitlebar && uiSceneData.scene)
    {
        uiSceneData.scene->calcSceneMVP();
        uiSceneData.scene->provideSceneMVP();
        uiSceneData.scene->render();
    }
    renderer->renderFinish();
}

void WindowWayland::updateWindowSize()
{
}

void WindowWayland::startDragging()
{
    if (!flagMouseLocked)
    {
        xdg_toplevel_move(toplevel_, seat_, serial_);
    }
}

void WindowWayland::stopDragging()
{
    updateCursor();
}

void WindowWayland::close()
{
    flagCloseRequested = true;
}

void WindowWayland::checkWindowLock()
{
    if (constraints_ && pointer_ && flagMouseCapture && !flagMouseLocked)
    {
        flagMouseLocked = true;

        locked =
            zwp_pointer_constraints_v1_lock_pointer(
                constraints_,
                surface_,
                pointer_,
                nullptr, // region
                ZWP_POINTER_CONSTRAINTS_V1_LIFETIME_PERSISTENT);

        wl_pointer_set_cursor(
            pointer_,
            serial_,
            nullptr, // no cursor surface
            0,
            0);
    }
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

void WindowWayland::subscribePointer(wl_pointer *pointer)
{
    this->pointer_ = pointer;
    static const wl_pointer_listener pointerListener = {
        .enter = handlePointerEnter,
        .leave = handlePointerLeave,
        .motion = handlePointerMotion,
        .button = handlePointerButton,
        .axis = handlePointerAxis};

    wl_pointer_add_listener(pointer, &pointerListener, this);

    relativePointer_ =
        zwp_relative_pointer_manager_v1_get_relative_pointer(
            relativePointerManager_,
            pointer);

    static const zwp_relative_pointer_v1_listener
        relativePointerListener = {
            .relative_motion = handlePointerRelativeMotion};

    zwp_relative_pointer_v1_add_listener(
        relativePointer_,
        &relativePointerListener,
        this);
}

void WindowWayland::subscribeKeyboard(wl_keyboard *keyboard)
{
    static const wl_keyboard_listener keyboardListener = {
        .keymap = hanldeKeyboardKeyMap,
        .enter = handleKeyboardEnter,
        .leave = handleKeyboardLeave,
        .key = handleKeyboardKey,
        .modifiers = handleKeyboardModifiers,
        .repeat_info = handleKeyboardRepeatInfo};

    wl_keyboard_add_listener(keyboard, &keyboardListener, this);
}

void WindowWayland::provideMousePosition(float x, float y)
{
    mouseX = (int32)(x * (float)scaleFactor);
    mouseY = (int32)(y * (float)scaleFactor);

    emitEventMouseMove(0, 0, mouseX, mouseY);
}

void WindowWayland::provideMouseShift(float shiftX, float shiftY)
{
    int32 intShiftX = (int32)(shiftX * (float)scaleFactor);
    int32 intShiftY = (int32)(shiftY * (float)scaleFactor);

    emitEventMouseMove(intShiftX, intShiftY, mouseX, mouseY);
}

void WindowWayland::provideMouseButton(MouseButton button, bool state, uint32 serial)
{
    this->serial_ = serial;
    emitEventMouseClick(state, button);
}

void WindowWayland::provideKeyboardKey(uint16 key, bool state)
{
    emitEventKey(state, key);
}

void WindowWayland::providePointerDataOnEnter(
    wl_surface *surface,
    wl_pointer *wl_pointer,
    uint32 serial)
{
    this->pointer_ = wl_pointer;
    this->serial_ = serial;
    updateCursor();
    checkWindowLock();
}

void WindowWayland::updateCursor()
{
    if (!serial_)
        return;

    wl_pointer *pointer = wl_seat_get_pointer(seat_);
    if (flagMouseLocked)
    {
        wl_pointer_set_cursor(pointer, serial_, nullptr, 0, 0);
    }
    else
    {
        wl_cursor_theme *theme =
            wl_cursor_theme_load(nullptr, 24, shm_);
        wl_cursor *cursor =
            wl_cursor_theme_get_cursor(theme, "left_ptr");
        wl_cursor_image *image = cursor->images[0];
        wl_buffer *buffer = wl_cursor_image_get_buffer(image);

        wl_pointer_set_cursor(pointer, serial_, cursorSurface_, 0, 0);

        wl_surface_attach(cursorSurface_, buffer, 0, 0);
        wl_surface_damage_buffer(
            cursorSurface_,
            0, 0,
            image->width,
            image->height);
        wl_surface_commit(cursorSurface_);
    }
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

void WindowWayland::handleToplevelClose(void *data, xdg_toplevel *toplevel)
{
    WindowWayland *window = static_cast<WindowWayland *>(data);
    window->close();
}

void WindowWayland::handleShellPing(void *data, xdg_wm_base *shell, uint32_t serial)
{
    xdg_wm_base_pong(shell, serial);
}

void WindowWayland::handleShellSurfaceConfigure(void *data, xdg_surface *shellSurface, uint32_t serial)
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

void WindowWayland::handleSeatCapabilities(
    void *data,
    wl_seat *seat,
    uint32 capabilities)
{
    WindowWayland *window = static_cast<WindowWayland *>(data);
    if (capabilities & WL_SEAT_CAPABILITY_POINTER)
    {
        wl_pointer *pointer = wl_seat_get_pointer(seat);
        window->subscribePointer(pointer);
    }

    if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD)
    {
        wl_keyboard *keyboard = wl_seat_get_keyboard(seat);
        window->subscribeKeyboard(keyboard);
    }
}

void WindowWayland::handleSeatName(
    void *data,
    wl_seat *seat,
    const char *name)
{
    // Logger::log << "Seat name: " << name << endl;
}

void WindowWayland::handlePointerEnter(void *data,
                                       wl_pointer *wl_pointer,
                                       uint32 serial,
                                       wl_surface *surface,
                                       wl_fixed_t surfaceX,
                                       wl_fixed_t surfaceY)
{
    WindowWayland *window = static_cast<WindowWayland *>(data);
    window->providePointerDataOnEnter(surface, wl_pointer, serial);
}

void WindowWayland::handlePointerLeave(void *data,
                                       wl_pointer *wl_pointer,
                                       uint32 serial,
                                       wl_surface *surface)
{
}

void WindowWayland::handlePointerMotion(void *data,
                                        wl_pointer *wl_pointer,
                                        uint32 time,
                                        wl_fixed_t surfaceX,
                                        wl_fixed_t surfaceY)
{
    WindowWayland *window = static_cast<WindowWayland *>(data);
    float x = (float)wl_fixed_to_double(surfaceX);
    float y = (float)wl_fixed_to_double(surfaceY);
    window->provideMousePosition(x, y);
}

void WindowWayland::handlePointerButton(void *data,
                                        wl_pointer *wl_pointer,
                                        uint32 serial,
                                        uint32 time,
                                        uint32 button,
                                        uint32 state)
{
    WindowWayland *window = static_cast<WindowWayland *>(data);
    if (button == 272)
        window->provideMouseButton(MouseButton::LeftMouseButton, state ? true : false, serial);
    if (button == 273)
        window->provideMouseButton(MouseButton::RightMouseButton, state ? true : false, serial);
    if (button == 274)
        window->provideMouseButton(MouseButton::MiddleMouseButton, state ? true : false, serial);
}

void WindowWayland::handlePointerAxis(void *data,
                                      wl_pointer *wl_pointer,
                                      uint32 time,
                                      uint32 axis,
                                      wl_fixed_t value)
{
}

void WindowWayland::handlePointerRelativeMotion(void *data,
                                                zwp_relative_pointer_v1 *relative_pointer,
                                                uint32 utime_hi,
                                                uint32 utime_lo,
                                                wl_fixed_t dx,
                                                wl_fixed_t dy,
                                                wl_fixed_t dx_unaccel,
                                                wl_fixed_t dy_unaccel)
{
    WindowWayland *window = static_cast<WindowWayland *>(data);
    float x = (float)wl_fixed_to_double(dx_unaccel);
    float y = (float)wl_fixed_to_double(dy_unaccel);

    window->provideMouseShift(x, y);
}

void WindowWayland::hanldeKeyboardKeyMap(void *data,
                                         wl_keyboard *wl_keyboard,
                                         uint32 format,
                                         int32 fd,
                                         uint32 size)
{
    ::close(fd);
}

void WindowWayland::handleKeyboardEnter(void *data,
                                        wl_keyboard *wl_keyboard,
                                        uint32 serial,
                                        wl_surface *surface,
                                        wl_array *keys)
{
}

void WindowWayland::handleKeyboardLeave(void *data,
                                        struct wl_keyboard *wl_keyboard,
                                        uint32 serial,
                                        struct wl_surface *surface)
{
}

void WindowWayland::handleKeyboardKey(void *data,
                                      wl_keyboard *wl_keyboard,
                                      uint32 serial,
                                      uint32 time,
                                      uint32 key,
                                      uint32 state)
{
    WindowWayland *window = static_cast<WindowWayland *>(data);
    window->provideKeyboardKey(key, state ? true : false);
}

void WindowWayland::handleKeyboardModifiers(void *data,
                                            struct wl_keyboard *wl_keyboard,
                                            uint32 serial,
                                            uint32 mods_depressed,
                                            uint32 mods_latched,
                                            uint32 mods_locked,
                                            uint32 group)
{
}

void WindowWayland::handleKeyboardRepeatInfo(void *data,
                                             struct wl_keyboard *wl_keyboard,
                                             int32 rate,
                                             int32 delay)
{
}

void WindowWayland::onRemoveRegistry(void *a, wl_registry *b, uint32_t c)
{
}

void WindowWayland::onRegistry(void *data,
                               wl_registry *registry,
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
    else if (!strcmp(interface, "wl_seat"))
    {
        window->seat_ = static_cast<wl_seat *>(wl_registry_bind(
            registry,
            name,
            &wl_seat_interface,
            1));
    }
    else if (!strcmp(interface, "zwp_pointer_constraints_v1"))
    {
        window->constraints_ =
            static_cast<zwp_pointer_constraints_v1 *>(
                wl_registry_bind(
                    registry,
                    name,
                    &zwp_pointer_constraints_v1_interface,
                    1));
    }
    else if (!strcmp(interface, "zwp_relative_pointer_manager_v1"))
    {
        window->relativePointerManager_ =
            static_cast<zwp_relative_pointer_manager_v1 *>(
                wl_registry_bind(
                    registry,
                    name,
                    &zwp_relative_pointer_manager_v1_interface,
                    1));
    }
    else if (!strcmp(interface, wl_shm_interface.name))
    {
        window->shm_ = static_cast<wl_shm *>(
            wl_registry_bind(registry,
                             name,
                             &wl_shm_interface,
                             1));
    }
}

#endif
