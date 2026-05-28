#include "features/scene/actorMesh.h"
#include "features/renderer/renderer.h"
#include <iostream>

using namespace wne;

ActorMesh::ActorMesh(Renderer *renderer, std::shared_ptr<Mesh> mesh) : Actor(renderer)
{
    this->mesh = std::move(mesh);
    objectId = this->mesh->genNewObjectId();
}

ActorMesh::~ActorMesh()
{
    this->mesh->freeObjectId(objectId);
}

void ActorMesh::setMaterial(std::shared_ptr<Material> material)
{
    this->material = std::move(material);
}

void ActorMesh::renderDepthShadow()
{
    Material *materialToUse = material ? material.get() : renderer->getDefaultMaterial().get();
    if (!materialToUse || !currentScene || objectId == 0xffffffff)
        return;

    auto state = renderer->getState();
    materialToUse->bindDepthShadow(objectId, renderer, state->getViewProjectionMatrix() * getModelMatrix(), getNormalMatrix(), mesh->getDataType());
    mesh->render(renderer->getFrameData());
}

void ActorMesh::renderDepth()
{
    Material *materialToUse = material ? material.get() : renderer->getDefaultMaterial().get();
    if (!materialToUse || materialToUse->getColorBlending() != ColorBlending::Solid || !currentScene || objectId == 0xffffffff)
        return;

    auto state = renderer->getState();
    materialToUse->bindDepth(objectId, state->getViewProjectionMatrix() * getModelMatrix(), getModelMatrix(), getNormalMatrix(), mesh->getDataType());
    mesh->render(renderer->getFrameData());
}

void ActorMesh::renderColor()
{
    Material *materialToUse = material ? material.get() : renderer->getDefaultMaterial().get();
    if (!materialToUse || !currentScene || objectId == 0xffffffff)
        return;

    auto state = renderer->getState();
    AffectingLights lights = currentScene->collectAffectingLights(getPosition(), 0.0f);
    materialToUse->bindColor(objectId, lights, state->getViewProjectionMatrix() * getModelMatrix(), getModelMatrix(), getNormalMatrix(), mesh->getDataType());
    mesh->render(renderer->getFrameData());
}

Actor::RenderPass ActorMesh::getRenderPass()
{
    return (!material || material->getColorBlending() == ColorBlending::Solid) ? RenderPass::Main : RenderPass::Blended;
}

float ActorMesh::getBoundingRadius()
{
    float size = getHighestAxisValue(scale);
    return mesh->getBoundingRadius() * size;
}
