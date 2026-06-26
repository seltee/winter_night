#include "features/os/wayland/waylandUIScene.h"
#include "features/scene/actorCamera.h"
#include "features/camera/cameraOrtho.h"
#include "features/scene/actorUI.h"
#include "features/logger/logger.h"

using namespace wne;

WaylandUIScene wne::createWaylandUIScene(Renderer *renderer, Window *window)
{
    auto sceneUI = Scene::create(renderer);

    auto cameraUI = CameraOrtho::create(512, 512);
    auto actorUICamera = sceneUI->createActor<ActorCamera>(cameraUI);
    sceneUI->setCamera(actorUICamera);

    auto actorUI = sceneUI->createActor<ActorUI>(window, 512, 512);
    auto root = &actorUI->getRoot();

    auto font = Font::create("Roboto-Medium.ttf");

    const wne::UINodeContainer::Decoration decorationTitleBar{
        .useBackgroundColor = true,
        .backgroundColor = 0xff44444a};

    auto caption = wne::UINodeText::create(font, "Application", 44, 0xff999999);

    // clang-format off
    root->setChild(
        wne::UINodeContainer::create(
            wne::UINodeCenter::create(
                {
                    caption
                }
            ),
            0, 56,
            decorationTitleBar
        )
    );
    // clang-format on

    return {
        sceneUI,
        cameraUI,
        actorUI,
        caption};
}