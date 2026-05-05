#include "engine.h"
#include <iostream>

int main()
{
    auto window = wne::Window::create(1920, 1080);

    if (!window)
        return 1;

    auto renderer = window->getRenderer();
    renderer->setSyncState(false);

    const std::vector<wne::VertexColored> vertices = {
        {{-50.0f, -50.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{50.0f, -50.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{50.0f, 50.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-50.0f, 50.0f, 0.0f}, {1.0f, 1.0f, 1.0f}}};

    const std::vector<uint16> indices = {
        0, 1, 2, 2, 3, 0};

    auto scene = wne::Scene::create();
    renderer->addScene(scene);
    auto model = wne::Model::createFromData(vertices, indices);
    auto mesh = renderer->createMesh(model);
    auto actorMesh = wne::ActorMesh::create(mesh);
    scene->addActor(actorMesh);
    auto camera = wne::CameraOrtho::createHeightBased(window, 240.0f);
    auto actorCamera = wne::ActorCamera::create(camera);
    scene->addActor(actorCamera);
    scene->setCamera(actorCamera);

    float translate = 0.0f;
    while (!window->isCloseRequested())
    {
        float delta = wne::Engine::getInstance()->update();
        translate += delta * 2.0f;
        actorMesh->setPosition(sinf(translate) * 80.0f, 0.0f, 0.0f);
        actorMesh->setRotation(0.0f, 0.0f, translate * 0.1f);
        float scale = sinf(translate * 0.4f) * 0.2f + 0.8f;
        actorMesh->setScale(scale, scale, 1.0f);
    }

    return 0;
}
