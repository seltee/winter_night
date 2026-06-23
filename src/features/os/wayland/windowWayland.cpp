#include "features/os/wayland/windowWayland.h"

#if defined(OS_LINUX)
#include "features/renderer/vulkan/rendererVulkanWayland.h"
#include "features/scene/actorCamera.h"
#include "features/camera/cameraOrtho.h"
#include "features/scene/actorUI.h"
#include "features/logger/logger.h"
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
    wl_display_dispatch_pending(display_);
    wl_display_flush(display_);
    renderer->update(delta);
    soundSystem->update();
}

void WindowWayland::render()
{
    renderer->renderStart();
    renderer->renderScenes();
    if (flagShowTitlebar)
    {
        if (!uiScene)
            uiScene = createUIScene();
        uiScene->provideSceneMVP();
        uiScene->render();
    }
    renderer->renderFinish();
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

void WindowWayland::subscribePointer(wl_pointer *pointer)
{

    static const wl_pointer_listener pointerListener = {
        .enter = handlePointerEnter,
        .leave = handlePointerLeave,
        .motion = handlePointerMotion,
        .button = handlePointerButton,
        .axis = handlePointerAxis};

    wl_pointer_add_listener(pointer, &pointerListener, this);
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
    int32 mouseX = (int32)(x * (float)scaleFactor);
    int32 mouseY = (int32)(y * (float)scaleFactor);
    int32 mouseDiffX = mouseX - this->mouseX;
    int32 mouseDiffY = mouseY - this->mouseY;

    emitEventMouseMove(mouseDiffX, mouseDiffY, mouseX, mouseY);
    this->mouseX = mouseX;
    this->mouseY = mouseY;
}

void WindowWayland::provideMouseButton(MouseButton button, bool state)
{
    emitEventMouseClick(state, button);
}

void WindowWayland::provideKeyboardKey(uint16 key, bool state)
{
    emitEventKey(state, key);
}

std::shared_ptr<Scene> WindowWayland::createUIScene()
{
    auto sceneUI = Scene::create(renderer.get());

    auto cameraUI = CameraOrtho::create(256, 256);
    auto actorUICamera = sceneUI->createActor<ActorCamera>(cameraUI);
    sceneUI->setCamera(actorUICamera);

    auto actorUI = sceneUI->createActor<ActorUI>(this, 256, 256);
    auto root = &actorUI->getRoot();

    auto font = Font::create("Roboto-Medium.ttf");
    // clang-format off
    root->setChild(
        wne::UINodeCenter::create(
            wne::UINodeText::create(font, "Application", 70, 0xff999999)
        )  
    );
    // clang-format on

    return sceneUI;
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
        Logger::log << "Pointer" << endl;
        wl_pointer *pointer = wl_seat_get_pointer(seat);
        window->subscribePointer(pointer);
    }

    if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD)
    {
        Logger::log << "Keyboard" << endl;
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
        window->provideMouseButton(MouseButton::LeftMouseButton, state ? true : false);
    if (button == 273)
        window->provideMouseButton(MouseButton::RightMouseButton, state ? true : false);
    if (button == 274)
        window->provideMouseButton(MouseButton::MiddleMouseButton, state ? true : false);
}

void WindowWayland::handlePointerAxis(void *data,
                                      wl_pointer *wl_pointer,
                                      uint32 time,
                                      uint32 axis,
                                      wl_fixed_t value)
{
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
    else if (strcmp(interface, "wl_seat") == 0)
    {
        window->seat_ = static_cast<wl_seat *>(wl_registry_bind(
            registry,
            name,
            &wl_seat_interface,
            1));
    }
}

#endif
