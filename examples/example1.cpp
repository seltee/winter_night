#include "engine.h"
#include <iostream>

int main()
{
    auto window = wne::Window::create(2560, 1440);

    if (!window)
        return 1;

    auto renderer = window->getRenderer();
    renderer->setSyncState(false);

    const std::vector<wne::VertexTextured> vertices = {
        {{-5.0f, -5.0f, 0.0f}, {0.0f, 1.0f}, {0, 0, -1.0f}},
        {{5.0f, -5.0f, 0.0f}, {1.0f, 1.0f}, {0, 0, -1.0f}},
        {{5.0f, 5.0f, 0.0f}, {1.0f, 0.0f}, {0, 0, -1.0f}},
        {{-5.0f, 5.0f, 0.0f}, {0.0f, 0.0f}, {0, 0, -1.0f}}};

    const std::vector<uint16_t> indices = {0, 2, 1, 3, 2, 0};

    // scene
    auto scene = wne::Scene::create();
    renderer->addScene(scene);
    scene->setAmbientLight(0.3f, 0.2f, 0.2f);

    // images
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
    auto model = wne::Model::createFromData(vertices, indices);
    auto mesh = renderer->createMesh(model);
    auto actorMesh = wne::ActorMesh::create(mesh);
    actorMesh->setMaterial(materialEx);
    scene->addActor(actorMesh);

    auto actorMesh2 = wne::ActorMesh::create(mesh);
    actorMesh2->setMaterial(materialEx);
    scene->addActor(actorMesh2);
    actorMesh2->setPosition(8.0f, 8.0f, 24.0f);

    auto actorMesh3 = wne::ActorMesh::create(mesh);
    actorMesh3->setMaterial(materialEx);
    scene->addActor(actorMesh3);
    actorMesh3->setPosition(-8.0f, 8.0f, 20.0f);

    auto plainModel = wne::Primitives::createPlain(4.0f, wne::normalize(wne::Vector3(-1.0f, 1.0f, -1.0f)));
    auto plainMesh = renderer->createMesh(plainModel);
    auto actorPlain = wne::ActorMesh::create(plainMesh);
    actorPlain->setMaterial(materialBox);
    scene->addActor(actorPlain);
    actorPlain->setPosition(12.0f, -4.0f, 12.0f);

    auto boxModel = wne::Primitives::createBox(4.0f);
    auto boxMesh = renderer->createMesh(boxModel);
    auto actorBox = wne::ActorMesh::create(boxMesh);
    actorBox->setMaterial(materialBox);
    scene->addActor(actorBox);
    actorBox->setPosition(-12.0f, 8.0f, 4.0f);

    auto actorBox2 = wne::ActorMesh::create(boxMesh);
    actorBox2->setMaterial(materialBox);
    scene->addActor(actorBox2);
    actorBox2->setPosition(-20.0f, 8.0f, 4.0f);

    auto actorTerrain = wne::ActorTerrain::createFromImage(100, 80.0f, 10.0f, imageBox, renderer);
    actorTerrain->setMaterial(materialBox);
    scene->addActor(actorTerrain);

    auto actorLoadedModel = wne::Loader3d::loadAsModel("./box.fbx");
    auto actorLoadedMesh = renderer->createMesh(actorLoadedModel);
    auto actorLoadedBox = wne::ActorMesh::create(actorLoadedMesh);
    actorLoadedBox->setMaterial(materialMetalBox);
    scene->addActor(actorLoadedBox);
    actorLoadedBox->setPosition(10.0f, 8.0f, 4.0f);

    // light
    auto sun = renderer->createLightDirectional();
    sun->setDirection({-0.5, -0.5, -0.5});
    sun->setColor({4.0f, 4.0f, 4.0f});
    auto actorSun = wne::ActorLight::create(sun);
    scene->addActor(actorSun);

    sun->enableShadows(1, 1024);
    auto textureSunShadow = sun->getCascadeAsTexture(0);
    auto materialSunShadow = renderer->createFlatMaterial(textureSunShadow);
    auto actorSunShadowBox = wne::ActorMesh::create(boxMesh);
    actorSunShadowBox->setMaterial(materialSunShadow);
    scene->addActor(actorSunShadowBox);
    actorSunShadowBox->setPosition(12.0f, 8.0f, 0.0f);

    auto spot = renderer->createLightSpot();
    spot->setDirection({0, -1.0f, 0});
    spot->setColor({2.0f, 2.0f, 10.0f});
    spot->setAffectRadius(60.0f);
    spot->setOuterCutOff(0.3f);
    auto actorSpot = wne::ActorLight::create(spot);
    scene->addActor(actorSpot);

    const int amountOfLights = 8;
    std::vector<std::shared_ptr<wne::ActorLight>> omniLights;
    for (int i = 0; i < amountOfLights; i++)
    {
        auto light = renderer->createLightOmni();
        light->setColor({fRand(0.5f, 5.0f), fRand(0.5f, 5.0f), fRand(0.5f, 5.0f)});
        light->setAffectRadius(10.0f);
        auto actorLight = wne::ActorLight::create(light);
        scene->addActor(actorLight);
        omniLights.push_back(actorLight);
    }

    // camera
    auto camera = wne::CameraPerspective::create(window);
    auto actorCamera = wne::ActorCameraFlying::create(camera, window);
    scene->addActor(actorCamera);
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
