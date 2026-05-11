#include "features/scene/actorMesh.h"
#include "features/renderer/renderer.h"
#include <iostream>

using namespace wne;

ActorMesh::ActorMesh(std::shared_ptr<Mesh> mesh)
{
    this->mesh = std::move(mesh);
}

std::shared_ptr<ActorMesh> ActorMesh::create(std::shared_ptr<Mesh> mesh)
{
    return std::make_shared<ActorMesh>(std::move(mesh));
}

void ActorMesh::setMaterial(std::shared_ptr<Material> material)
{
    this->material = std::move(material);
}

void ActorMesh::render(Renderer *renderer)
{
    if (!material)
        return;
    material->bind(renderer->getViewProjectionMatrix() * getModelMatrix(), getNormalMatrix(), mesh->getDataType());
    mesh->render(renderer->getFrameData());
}
