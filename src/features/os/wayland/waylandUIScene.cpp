#include "features/os/wayland/waylandUIScene.h"
#include "features/scene/actorCamera.h"
#include "features/camera/cameraOrtho.h"
#include "features/scene/actorUI.h"
#include "features/logger/logger.h"
#include "engine.h"

using namespace wne;

WaylandUIScene wne::createWaylandUIScene(Renderer *renderer, Window *window)
{
    auto sceneUI = Scene::create(renderer);

    auto cameraUI = CameraOrtho::create(512, 512);
    auto actorUICamera = sceneUI->createActor<ActorCamera>(cameraUI);
    sceneUI->setCamera(actorUICamera);

    auto actorUI = sceneUI->createActor<ActorUI>(window, 512, 512);
    auto root = &actorUI->getRoot();

    const wne::UINodeContainer::Decoration decorationTitleBar{
        .useBackgroundColor = true,
        .backgroundColor = 0xff44444a};

    auto caption = wne::UINodeText::create(nullptr, "Application", 52, 0xffcccccc);

    auto iconCrossImage = Engine::getInstance()->media.getIconCross();
    auto iconCrossTexture = renderer->createTexture(iconCrossImage);

    auto iconCrossHoverImage = Engine::getInstance()->media.getIconCrossHover();
    auto iconCrossHoverTexture = renderer->createTexture(iconCrossHoverImage);

    // clang-format off
    root->setChild(
        wne::UINodeContainer::create(
            wne::UINodeRow::create(
                {
                    wne::UINodeCenter::create(nullptr, 76, 64),
                    wne::UINodeCenter::create(
                        {
                            caption
                        },
                        0, 48
                    ),
                    wne::UINodeCenter::create(
                        wne::UINodeButton::create(
                            wne::UINodeImage::create(iconCrossTexture),
                            wne::UINodeImage::create(iconCrossHoverTexture),
                            [&](wne::UINodeButton *button)
                            {
                                window->close();
                            }
                        ),
                        76, 64
                    ),
                },
                wne::UINodeRow::Layout::SpaceBetween
            ),
            0, 64,
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