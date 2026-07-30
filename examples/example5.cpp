#include "engine.h"

int main()
{
    wne::Engine::getInstance()->enableLogger();

    auto window = wne::Window::create("Example 5", 2560, 1440);
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

    auto imageMetalBox = wne::Image::create("./mbox.png");
    auto textureMetalBox = renderer->createTexture(imageMetalBox);
    auto materialMetalBox = renderer->createFlatMaterial(textureMetalBox);

    // actor with mesh
    auto animBoxData = wne::Loader3d::loadAsModelAnimation("./anim_box.fbx");
    auto animBoxCollection = renderer->createMeshCollection(animBoxData.models);
    auto actorLoadedBox = scene->createActor<wne::ActorAnimatedMesh>(animBoxCollection);
    actorLoadedBox->setMaterialToAll(materialBox);
    actorLoadedBox->setMaterialByName("Cube", materialMetalBox);
    actorLoadedBox->setDebugViewChierarchy(true);
    actorLoadedBox->setPosition(0.0f, 6.0f, 12.0f);

    for (auto &anim : animBoxData.animations)
    {
        auto animationTrack = actorLoadedBox->createAnimationTrack(anim);
        animationTrack->play(true);
    }

    auto animTubeData = wne::Loader3d::loadAsModelAnimation("./bones_tube.fbx");
    auto animTubeCollection = renderer->createMeshCollection(animTubeData.models);
    auto actorLoadedTube = scene->createActor<wne::ActorAnimatedMesh>(animTubeCollection);
    actorLoadedTube->setMaterialToAll(materialBox);
    actorLoadedTube->setDebugViewChierarchy(true);
    actorLoadedTube->setPosition(-4.0f, 1.0f, 2.0f);

    for (auto &anim : animTubeData.animations)
    {
        auto animationTrack = actorLoadedTube->createAnimationTrack(anim);
        animationTrack->play(true);
    }

    auto actorLoadedTube2 = scene->createActor<wne::ActorAnimatedMesh>(animTubeCollection);
    actorLoadedTube2->setMaterialToAll(materialBox);
    actorLoadedTube2->setDebugViewChierarchy(true);
    actorLoadedTube2->setPosition(6.0f, 1.0f, 2.0f);

    for (auto &anim : animTubeData.animations)
    {
        auto animationTrack = actorLoadedTube2->createAnimationTrack(anim);
        animationTrack->play(true);
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
    float r = 0;
    while (!window->isCloseRequested())
    {
        float delta = wne::Engine::getInstance()->update();
        goblinAnim += delta * 6.0f;
        translate += delta * 2.0f;

        r += delta;
        float sx = sin(r) * 2.0f;
        float sz = cos(r) * 2.0f;
        wne::Vector3 rPosFrom(sx, 1.0f, sz);
        wne::Vector3 rPosTo(-sx, 1.0f, -sz);
        renderer->addDebugLine(rPosFrom, rPosTo, wne::Renderer::DebugColor::Green);
        renderer->addDebugLine(rPosFrom, wne::Vector3(8.0f, 0, 10.0f), wne::Renderer::DebugColor::White);

        sun->setDirection({sinf(translate * 0.5f), 1.6, cosf(translate * 0.5f)});
    }

    return 0;
}
