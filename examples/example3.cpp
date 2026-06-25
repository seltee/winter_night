#include "engine.h"

std::shared_ptr<wne::UINodeButton> createButton(
    const char *label,
    std::shared_ptr<wne::Font> font,
    const std::function<void(wne::UINodeButton *)> &action)
{
    const wne::UINodeContainer::Decoration decorationNormal{
        .useBackgroundColor = true,
        .backgroundColor = 0xff777777};
    const wne::UINodeContainer::Decoration decorationHover{
        .useBackgroundColor = true,
        .backgroundColor = 0xffbbbbbb};

    const uint buttonWidth = 360;
    const uint buttonHeight = 80;

    // clang-format off
    return wne::UINodeButton::create(
        wne::UINodeContainer::create(
            wne::UINodeCenter::create(
                wne::UINodeText::create(font, label, 70, 0xffbbbbbb)
            ),
            buttonWidth, buttonHeight,
            decorationNormal
        ),
        wne::UINodeContainer::create(
            wne::UINodeCenter::create(
                wne::UINodeText::create(font, label, 70, 0xffffffff)
            ),
            buttonWidth, buttonHeight,
            decorationHover
        ),
        action);
    // clang-format on
}

int main()
{
    wne::Engine::getInstance()->enableLogger();

    auto window = wne::Window::create("Example 3", 2560, 1440);
    if (!window)
        return 1;

    auto renderer = window->getRenderer();
    renderer->setSyncState(false);

    // scene
    auto scene = renderer->createScene();
    scene->setAmbientLight(0.0f, 0.0f, 0.0f);

    // images
    auto imageAtmosphere = wne::ImageAtmo::create("./atmosphere.jpg");
    scene->setAtmosphere(
        renderer->createTexture(imageAtmosphere),
        renderer->createTexture(imageAtmosphere->getAtmosphereAsImage()));

    auto imageCog = wne::Image::create("./cog.png");
    auto textureCog = renderer->createTexture(imageCog);

    auto imageBox = wne::Image::create("./box.png");
    auto textureBox = renderer->createTexture(imageBox);
    auto materialBox = renderer->createFlatMaterial(textureBox);

    // actor with mesh
    auto sphereModel = wne::Primitives::createSphere(4.0f, 10, 10);
    auto sphereMesh = renderer->createMesh(sphereModel);
    auto sphereActor = scene->createActor<wne::ActorMesh>(sphereMesh);
    sphereActor->setPosition(-12.0f, 9.0f, -6.0f);

    auto boxModel = wne::Primitives::createBox(4.0f);
    auto boxMesh = renderer->createMesh(boxModel);
    auto actorBox = scene->createActor<wne::ActorMesh>(boxMesh);
    actorBox->setPosition(12.0f, 9.0f, -6.0f);

    auto actorTerrain = scene->createActor<wne::ActorTerrain>(100, 80.0f, 10.0f, imageBox);
    actorTerrain->setMaterial(materialBox);

    // light
    auto sun = renderer->createLightDirectional();
    sun->setDirection({-0.5, -0.5, -0.5});
    sun->setColor({4.0f, 4.0f, 4.0f});
    auto actorSun = scene->createActor<wne::ActorLight>(sun);

    // camera
    auto camera = wne::CameraPerspective::create(window);
    auto actorCamera = scene->createActor<wne::ActorCameraFlying>(camera, window);
    actorCamera->setPosition(wne::Vector3(0, 9.0f, 0));
    scene->setCamera(actorCamera);

    auto font = wne::Font::create("Roboto-Medium.ttf");
    auto text = renderer->createText(font);
    text->setText("Some Text");
    text->setFontSize(80);
    text->update();
    auto materialText = renderer->createFlatMaterial(text->getTexture());
    materialText->setColorBlending(wne::ColorBlending::Alpha);
    materialText->setLighted(false);

    auto actorSprite = scene->createActor<wne::ActorSprite>();
    actorSprite->setMaterial(materialText);
    actorSprite->setScale(4.0f * text->getProportion(), 4.0f, 1.0f);
    actorSprite->setPosition(0.0f, 16.0f, 12.0f);

    // ui
    auto ui = renderer->createScene();

    auto cameraUI = wne::CameraOrtho::create(window);
    auto actorUICamera = ui->createActor<wne::ActorCamera>(cameraUI);
    ui->setCamera(actorUICamera);

    auto actorUI = ui->createActor<wne::ActorUI>(window, 2560, 1440);
    auto root = &actorUI->getRoot();

    const uint padding = 32;

    // clang-format off
    root->setChild(
        wne::UINodeCenter::create(
            wne::UINodeColumn::create(
                {
                    createButton("Forward", font, 
                        [&](wne::UINodeButton *button){
                            std::cout << "Forward" << std::endl;
                            actorCamera->moveForward(8.0f);
                        }
                    ),
                    wne::UINodeContainer::create(padding, padding),
                    wne::UINodeRow::create(
                        {
                            createButton("Left", font, 
                                [&](wne::UINodeButton *button){
                                    std::cout << "Left" << std::endl;
                                    actorCamera->moveRight(-8.0f);
                                }
                            ),
                            wne::UINodeContainer::create(padding, padding),
                            createButton("Right", font, 
                                [&](wne::UINodeButton *button){
                                    std::cout << "Right" << std::endl;
                                    actorCamera->moveRight(8.0f);
                                }
                            ),
                        },
                        wne::UINodeRow::Layout::Middle
                    ),
                    wne::UINodeContainer::create(padding, padding),
                    createButton("Backwards", font, 
                        [&](wne::UINodeButton *button){
                            std::cout << "Backward" << std::endl;
                            actorCamera->moveForward(-8.0f);
                        }
                    ),
                    wne::UINodeContainer::create(padding, padding),
                    wne::UINodeImage::create(textureCog)
                },
                wne::UINodeColumn::Layout::Middle,
                wne::UINodeColumn::Position::Middle
            )
        )
    );
    // clang-format on

    float translate = 0.0f;
    while (!window->isCloseRequested())
    {
        float delta = wne::Engine::getInstance()->update();
        translate += delta * 2.0f;

        sun->setDirection({sinf(translate * 0.5f), 1.6, cosf(translate * 0.5f)});
    }

    return 0;
}
