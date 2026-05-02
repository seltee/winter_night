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
    auto objectMesh = wne::ObjectMesh::create(mesh);
    scene->addObject(objectMesh);
    auto camera = wne::CameraOrtho::createHeightBased(window, 240.0f);
    auto objectCamera = wne::ObjectCamera::create(camera);
    scene->addObject(objectCamera);
    scene->setCamera(objectCamera);

    while (!window->isCloseRequested())
    {
        wne::Engine::getInstance()->update();
    }

    return 0;
}
