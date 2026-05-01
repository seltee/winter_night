#include "engine.h"
#include <stdio.h>

int main()
{
    auto window = wne::Window::create(1920, 1080);

    if (!window)
        return 1;

    auto renderer = window->getRenderer();
    renderer->setSyncState(false);

    const std::vector<wne::VertexColored> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};

    const std::vector<uint16> indices = {
        0, 1, 2, 2, 3, 0};

    auto scene = wne::Scene::create();
    renderer->addScene(scene);
    auto model = wne::Model::createFromData(vertices, indices);
    auto mesh = renderer->createMesh(model);
    auto object = wne::ObjectMesh::create(mesh);
    scene->addObject(object);

    while (!window->isCloseRequested())
    {
        wne::Engine::getInstance()->update();
    }

    return 0;
}
