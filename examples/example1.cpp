#include "engine.h"
#include <iostream>

int main()
{
    auto window = wne::Window::create(2560, 1440);
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

    auto imageExplosion = wne::Image::create("./explosion.png");
    auto textureExplosion = renderer->createTexture(imageExplosion);
    auto materialExplosion = renderer->createFlatMaterial(textureExplosion);
    materialExplosion->setMasked(true);
    // materialExplosion->setLighted(false);
    materialExplosion->setColorBlending(wne::ColorBlending::Addition);

    // actor with mesh
    auto model = wne::Model::createFromData(vertices, indices);
    auto mesh = renderer->createMesh(model);
    auto actorMesh = scene->createActor<wne::ActorMesh>(mesh);
    actorMesh->setMaterial(materialEx);

    auto actorMesh2 = scene->createActor<wne::ActorMesh>(mesh);
    actorMesh2->setMaterial(materialEx);
    actorMesh2->setPosition(8.0f, 8.0f, 24.0f);

    auto actorMesh3 = scene->createActor<wne::ActorMesh>(mesh);
    actorMesh3->setMaterial(materialEx);
    actorMesh3->setPosition(-8.0f, 8.0f, 20.0f);

    auto plainModel = wne::Primitives::createPlain(4.0f, wne::normalize(wne::Vector3(-1.0f, 1.0f, -1.0f)));
    auto plainMesh = renderer->createMesh(plainModel);
    auto actorPlain = scene->createActor<wne::ActorMesh>(plainMesh);
    actorPlain->setMaterial(materialBox);
    actorPlain->setPosition(12.0f, -4.0f, 12.0f);

    auto actorSprite = scene->createActor<wne::ActorSprite>();
    actorSprite->setMaterial(materialExplosion);
    actorSprite->setScale(6.0f);
    actorSprite->setPosition(0.0f, 16.0f, 12.0f);

    auto boxModel = wne::Primitives::createBox(4.0f);
    auto boxMesh = renderer->createMesh(boxModel);
    auto actorBox = scene->createActor<wne::ActorMesh>(boxMesh);
    actorBox->setPosition(-12.0f, 8.0f, 4.0f);

    auto sphereModel = wne::Primitives::createSphere(4.0f, 10, 10);
    auto sphereMesh = renderer->createMesh(sphereModel);
    auto sphereBox = scene->createActor<wne::ActorMesh>(sphereMesh);
    sphereBox->setPosition(-12.0f, 8.0f, -6.0f);

    auto actorBox2 = scene->createActor<wne::ActorMesh>(boxMesh);
    actorBox2->setMaterial(materialBox);
    actorBox2->setPosition(-20.0f, 8.0f, 4.0f);
    actorBox2->setParent(actorMesh.get());

    auto actorTerrain = scene->createActor<wne::ActorTerrain>(100, 80.0f, 10.0f, imageBox);
    actorTerrain->setMaterial(materialBox);

    auto actorLoadedModel = wne::Loader3d::loadAsModel("./box.fbx");
    auto actorLoadedMesh = renderer->createMesh(actorLoadedModel);
    auto actorLoadedBox = scene->createActor<wne::ActorMesh>(actorLoadedMesh);
    actorLoadedBox->setMaterial(materialMetalBox);
    actorLoadedBox->setPosition(10.0f, 8.0f, 4.0f);

    // light
    auto sun = renderer->createLightDirectional();
    sun->setDirection({-0.5, -0.5, -0.5});
    sun->setColor({4.0f, 4.0f, 4.0f});
    auto actorSun = scene->createActor<wne::ActorLight>(sun);

    sun->enableShadows(1, 2048);
    auto textureSunShadow = sun->getCascadeAsTexture(0);
    auto materialSunShadow = renderer->createFlatMaterial(textureSunShadow);
    auto actorSunShadowBox = scene->createActor<wne::ActorMesh>(boxMesh);
    actorSunShadowBox->setMaterial(materialSunShadow);
    actorSunShadowBox->setPosition(12.0f, 8.0f, 0.0f);

    auto spot = renderer->createLightSpot();
    spot->setDirection({0, -1.0f, 0});
    spot->setColor({2.0f, 2.0f, 10.0f});
    spot->setAffectRadius(60.0f);
    spot->setOuterCutOff(0.3f);
    auto actorSpot = scene->createActor<wne::ActorLight>(spot);

    const int amountOfLights = 8;
    std::vector<std::shared_ptr<wne::ActorLight>> omniLights;
    for (int i = 0; i < amountOfLights; i++)
    {
        auto light = renderer->createLightOmni();
        light->setColor({fRand(0.5f, 5.0f), fRand(0.5f, 5.0f), fRand(0.5f, 5.0f)});
        light->setAffectRadius(10.0f);
        auto actorLight = scene->createActor<wne::ActorLight>(light);
        omniLights.push_back(actorLight);
    }

    // camera
    auto camera = wne::CameraPerspective::create(window);
    auto actorCamera = scene->createActor<wne::ActorCameraFlying>(camera, window);
    scene->setCamera(actorCamera);

    float translate = 0.0f;
    while (!window->isCloseRequested())
    {
        float delta = wne::Engine::getInstance()->update();
        translate += delta * 2.0f;
        actorMesh->setPosition(-8.0f, 8.0f, 1.0f);
        actorMesh->setRotation(0.0f, 0.0f, translate * 0.1f);
        float scale = sinf(translate * 0.4f) * 0.2f + 0.8f;
        actorMesh->setScale(scale, scale, 1.0f);

        sun->setDirection({sinf(translate * 0.5f), 1.6, cosf(translate * 0.5f)});

        actorLoadedBox->setRotation(0, translate * 0.1f, 0);

        actorSpot->setPosition(sinf(-translate * 0.5f) * 25.0f, 12.0, cosf(-translate * 0.5f) * 25.0f);

        float step = 1.0f / (float)amountOfLights;
        for (uint i = 0; i < omniLights.size(); i++)
        {
            float angle = (float)i * step * PI2;
            omniLights[i]->setPosition(sinf(angle + translate * 0.5f) * 40.0f, 6.0, cosf(angle + translate * 0.5f) * 40.0f);
        }
    }

    return 0;
}
