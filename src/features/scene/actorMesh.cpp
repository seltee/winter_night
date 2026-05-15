#include "features/scene/actorMesh.h"
#include "features/renderer/renderer.h"
#include <iostream>

using namespace wne;

ActorMesh::ActorMesh(std::shared_ptr<Mesh> mesh)
{
    this->mesh = std::move(mesh);
    objectId = this->mesh->genNewObjectId();
}

ActorMesh::~ActorMesh()
{
    this->mesh->freeObjectId(objectId);
}

std::shared_ptr<ActorMesh> ActorMesh::create(std::shared_ptr<Mesh> mesh)
{
    return std::make_shared<ActorMesh>(std::move(mesh));
}

void ActorMesh::setMaterial(std::shared_ptr<Material> material)
{
    this->material = std::move(material);
}

void ActorMesh::renderDepthShadow(Renderer *renderer)
{
    if (!material || !currentScene)
        return;
    if (objectId == 0xffffffff)
        return;

    auto state = renderer->getState();
    material->bindDepthShadow(objectId, renderer, state->getViewProjectionMatrix() * getModelMatrix(), getNormalMatrix(), mesh->getDataType());
    mesh->render(renderer->getFrameData());
}

void ActorMesh::renderDepth(Renderer *renderer)
{
    if (!material || !currentScene)
        return;
    if (objectId == 0xffffffff)
        return;

    auto state = renderer->getState();
    material->bindDepth(objectId, state->getViewProjectionMatrix() * getModelMatrix(), getNormalMatrix(), mesh->getDataType());
    mesh->render(renderer->getFrameData());
}

void ActorMesh::renderColor(Renderer *renderer)
{
    if (!material || !currentScene)
        return;
    if (objectId == 0xffffffff)
        return;

    auto state = renderer->getState();
    AffectingLights lights = currentScene->collectAffectingLights();
    material->bindColor(objectId, lights, state->getViewProjectionMatrix() * getModelMatrix(), getNormalMatrix(), mesh->getDataType());
    mesh->render(renderer->getFrameData());
}
