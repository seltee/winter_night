#include "engine.h"
#include <stdio.h>

int main()
{
    auto window = WNE::Window::create(1920, 1080);

    if (!window)
        return 1;

    auto renderer = window->getRenderer();
    renderer->setSyncState(false);

    const std::vector<VertexColored> vertices = {
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 0.0f}},
    };

    auto scene = WNE::Scene::create();
    renderer->addScene(scene);
    auto model = WNE::Model::createFromData(vertices);
    auto mesh = renderer->createMesh(model);
    auto object = WNE::ObjectMesh::create(mesh);
    scene->addObject(object);

    while (!window->isCloseRequested())
    {
        WNE::Engine::getInstance()->update();
    }

    return 0;
}
