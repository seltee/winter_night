#include "engine.h"
#include <iostream>

int main()
{
    auto window = wne::Window::create(2560, 1440);

    if (!window)
        return 1;

    auto renderer = window->getRenderer();
    renderer->setSyncState(false);

    /*
    const std::vector<wne::VertexColored> vertices = {
        {{-50.0f, -50.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{50.0f, -50.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{50.0f, 50.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-50.0f, 50.0f, 0.0f}, {1.0f, 1.0f, 1.0f}}};
    */

    const std::vector<wne::VertexTextured> vertices = {
        {{-5.0f, -5.0f, 0.0f}, {0.0f, 0.0f}},
        {{5.0f, -5.0f, 0.0f}, {1.0f, 0.0f}},
        {{5.0f, 5.0f, 0.0f}, {1.0f, 1.0f}},
        {{-5.0f, 5.0f, 0.0f}, {0.0f, 1.0f}}};

    const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};

    // scene
    auto scene = wne::Scene::create();
    renderer->addScene(scene);

    // actor with mesh
    auto model = wne::Model::createFromData(vertices, indices);
    auto mesh = renderer->createMesh(model);
    auto image = wne::Image::create("./ex.png");
    auto texture = renderer->createTexture(image);
    auto material = renderer->createFlatMaterial(texture);
    auto actorMesh = wne::ActorMesh::create(mesh);
    actorMesh->setMaterial(material);
    scene->addActor(actorMesh);

    auto actorMesh2 = wne::ActorMesh::create(mesh);
    actorMesh2->setMaterial(material);
    scene->addActor(actorMesh2);
    actorMesh2->setPosition(8.0f, 8.0f, 8.0f);

    auto actorMesh3 = wne::ActorMesh::create(mesh);
    actorMesh3->setMaterial(material);
    scene->addActor(actorMesh3);
    actorMesh3->setPosition(-8.0f, 8.0f, 8.0f);

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
        actorMesh->setPosition(0.0f, 0.0f, 1.0f);
        actorMesh->setRotation(0.0f, 0.0f, translate * 0.1f);
        float scale = sinf(translate * 0.4f) * 0.2f + 0.8f;
        actorMesh->setScale(scale, scale, 1.0f);
    }

    return 0;
}
