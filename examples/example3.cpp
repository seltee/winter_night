#include "engine.h"

int main()
{
    wne::Engine::getInstance()->enableLogger();

    auto window = wne::Window::create(2560, 1440);
    if (!window)
        return 1;

    window->setMouseCapture(true);
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

    auto imageEx = wne::Image::create("./ex.png");
    auto textureEx = renderer->createTexture(imageEx);
    auto materialEx = renderer->createFlatMaterial(textureEx);

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

    auto actorUI = ui->createActor<wne::ActorUI>(2560, 1440);
    auto root = &actorUI->getRoot();

    // clang-format off
    root->setChild(
        wne::UINodeColumn::create(
            {
            wne::UINodeCenter::create(
                 wne::UINodeText::create(font, "Centered text", 70),
                 400, 72),
            wne::UINodeCenter::create(
                 wne::UINodeText::create(font, "Second line", 70),
                 400, 72),
            wne::UINodeCenter::create(
                 wne::UINodeText::create(font, "Ending", 70),
                 400, 72)
            },
            wne::UINodeColumn::Layout::Middle,
            wne::UINodeColumn::Position::Middle
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
