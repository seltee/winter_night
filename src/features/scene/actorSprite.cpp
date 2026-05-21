#include "features/scene/actorSprite.h"
#include "features/renderer/mesh.h"
#include "features/renderer/renderer.h"
#include <iostream>

using namespace wne;

ActorSprite::ActorSprite(Renderer *renderer)
{
    mesh = renderer->getDefaultPlain();
    objectId = mesh->genNewObjectId();
    isShadowEnabled = false;
}

ActorSprite::~ActorSprite()
{
    mesh->freeObjectId(objectId);
}

std::shared_ptr<ActorSprite> ActorSprite::create(Renderer *renderer)
{
    return std::make_shared<ActorSprite>(renderer);
}

const Matrix4x4 &ActorSprite::getModelMatrix()
{
    if (currentScene)
    {
        Matrix4x4 newModel = Matrix4x4::translation(position);
        newModel = newModel * Matrix4x4(lookAt(position, currentScene->getCameraActor()->getPosition()));
        mModel = newModel * Matrix4x4::scale(scale);
    }
    return mModel;
}

void ActorSprite::update(float delta)
{
    isDirtyFlag = true;
    eventUpdate(delta);
}

void ActorSprite::setMaterial(std::shared_ptr<Material> material)
{
    this->material = std::move(material);
}

void ActorSprite::renderDepthShadow(Renderer *renderer)
{
    Material *materialToUse = material ? material.get() : renderer->getDefaultMaterial().get();
    if (!materialToUse || !currentScene || objectId == 0xffffffff)
        return;

    auto state = renderer->getState();
    materialToUse->bindDepthShadow(objectId, renderer, state->getViewProjectionMatrix() * getModelMatrix(), getNormalMatrix(), mesh->getDataType());
    mesh->render(renderer->getFrameData());
}

void ActorSprite::renderDepth(Renderer *renderer)
{
    Material *materialToUse = material ? material.get() : renderer->getDefaultMaterial().get();
    if (!materialToUse || materialToUse->getColorBlending() != ColorBlending::Solid || !currentScene || objectId == 0xffffffff)
        return;

    auto state = renderer->getState();
    materialToUse->bindDepth(objectId, state->getViewProjectionMatrix() * getModelMatrix(), getModelMatrix(), getNormalMatrix(), mesh->getDataType());
    mesh->render(renderer->getFrameData());
}

void ActorSprite::renderColor(Renderer *renderer)
{
    Material *materialToUse = material ? material.get() : renderer->getDefaultMaterial().get();
    if (!materialToUse || !currentScene || objectId == 0xffffffff)
        return;

    auto state = renderer->getState();
    AffectingLights lights = materialToUse->isLighted() ? currentScene->collectAffectingLights() : AffectingLights{};
    materialToUse->bindColor(objectId, lights, state->getViewProjectionMatrix() * getModelMatrix(), getModelMatrix(), getNormalMatrix(), mesh->getDataType());
    mesh->render(renderer->getFrameData());
}

Actor::RenderPass ActorSprite::getRenderPass()
{
    return (!material || material->getColorBlending() == ColorBlending::Solid) ? RenderPass::Main : RenderPass::Blended;
}