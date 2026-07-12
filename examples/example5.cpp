#include "engine.h"

int main()
{
    wne::Engine::getInstance()->enableLogger();

    auto window = wne::Window::create("Example 1", 2560, 1440);
    if (!window)
        return 1;

    window->setMouseCapture(true);
    auto renderer = window->getRenderer();
    renderer->setSyncState(false);

    const std::vector<wne::VertexTextured> vertices = {
        {{-5.0f, -5.0f, 0.0f}, {0.0f, 1.0f}, {0, 0, -1.0f}},
        {{5.0f, -5.0f, 0.0f}, {1.0f, 1.0f}, {0, 0, -1.0f}},
        {{5.0f, 5.0f, 0.0f}, {1.0f, 0.0f}, {0, 0, -1.0f}},
        {{-5.0f, 5.0f, 0.0f}, {0.0f, 0.0f}, {0, 0, -1.0f}}};

    const std::vector<uint16_t> indices = {0, 2, 1, 3, 2, 0};

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

    auto imageMetalBox = wne::Image::create("./mbox.png");
    auto textureMetalBox = renderer->createTexture(imageMetalBox);
    auto materialMetalBox = renderer->createFlatMaterial(textureMetalBox);

    // actor with mesh
    auto animBoxData = wne::Loader3d::loadAsModelAnimation("./anim_box.fbx");
    auto animBoxCollection = renderer->createMeshCollection(animBoxData.models);
    auto actorLoadedBox = scene->createActor<wne::ActorAnimatedMesh>(animBoxCollection);
    auto animationTrack = actorLoadedBox->createAnimationTrack(animBoxData.animations[0]);
    animationTrack->play();
    // actorLoadedBox->setPosition(0.0f, 6.0f, 4.0f);

    auto actorLoadedBoxStatic = scene->createActor<wne::ActorMesh>((*animBoxCollection)[0]);
    actorLoadedBoxStatic->setPosition(8.0f, 0, 10.0f);

    wne::Logger::log << "Loaded objects" << wne::endl;
    for (auto &model : animBoxData.models)
    {
        wne::Logger::log << model->getName() << wne::endl;
    }

    wne::Logger::log << "Loaded animations" << wne::endl;
    for (auto &anim : animBoxData.animations)
    {
        wne::Logger::log << anim->getName() << wne::endl;
        if (anim->getAnimationTarget("Cube"))
        {
            wne::Logger::log << "Targets Cube" << wne::endl;
        }
    }

    // light
    auto sun = renderer->createLightDirectional();
    sun->setDirection({-0.5, -0.5, -0.5});
    sun->setColor({4.0f, 4.0f, 4.0f});
    auto actorSun = scene->createActor<wne::ActorLight>(sun);
    sun->enableShadows(1, 2048);

    // camera
    auto camera = wne::CameraPerspective::create(window);
    auto actorCamera = scene->createActor<wne::ActorCameraFlying>(camera, window);
    scene->setCamera(actorCamera);

    float translate = 0.0f;
    float goblinAnim = 0.0f;
    while (!window->isCloseRequested())
    {
        float delta = wne::Engine::getInstance()->update();
        goblinAnim += delta * 6.0f;

        translate += delta * 2.0f;

        sun->setDirection({sinf(translate * 0.5f), 1.6, cosf(translate * 0.5f)});
    }

    return 0;
}
