#include "features/scene/actorSprite.h"
#include "features/renderer/mesh.h"
#include "features/renderer/renderer.h"
#include <iostream>

using namespace wne;

ActorSprite::ActorSprite(Renderer *renderer) : Actor(renderer)
{
    mesh = renderer->getDefaultPlane();
    objectId = mesh->genNewObjectId();
    isShadowEnabled = false;
}

ActorSprite::~ActorSprite()
{
    mesh->freeObjectId(objectId);
}

const Matrix4x4 &ActorSprite::getModelMatrix()
{
    if (currentScene)
    {
        if (parent)
        {
            Matrix4x4 translation = Matrix4x4::translation(position);
            Matrix4x4 mPosition = parent->getModelMatrix() * translation;
            Vector4 absolutePosition = mPosition * Vector4(0, 0, 0, 1.0f);
            absolutePosition = absolutePosition / absolutePosition.w;

            Matrix4x4 newModel = Matrix4x4::translation(absolutePosition);
            newModel = newModel * Matrix4x4(lookAt(absolutePosition.xyz(), currentScene->getCameraActor()->getPosition()));
            mModel = newModel * Matrix4x4::scale(scale);
        }
        else
        {
            Matrix4x4 newModel = Matrix4x4::translation(position);
            newModel = newModel * Matrix4x4(lookAt(position, currentScene->getCameraActor()->getPosition()));
            mModel = newModel * Matrix4x4::scale(scale);
        }
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

void ActorSprite::renderDepthShadow()
{
    Material *materialToUse = material ? material.get() : renderer->getDefaultMaterial().get();
    if (!materialToUse || !currentScene || objectId == 0xffffffff)
        return;

    auto state = renderer->getState();
    materialToUse->bindDepthShadow(objectId, renderer, state->getViewProjectionMatrix() * getModelMatrix(), getNormalMatrix(), mesh->getDataType());
    mesh->render(renderer->getFrameData());
}

void ActorSprite::renderDepth()
{
    Material *materialToUse = material ? material.get() : renderer->getDefaultMaterial().get();
    if (!materialToUse || materialToUse->getColorBlending() != ColorBlending::Solid || !currentScene || objectId == 0xffffffff)
        return;

    auto state = renderer->getState();
    materialToUse->bindDepth(objectId, state->getViewProjectionMatrix() * getModelMatrix(), getModelMatrix(), getNormalMatrix(), mesh->getDataType());
    mesh->render(renderer->getFrameData());
}

void ActorSprite::renderColor()
{
    Material *materialToUse = material ? material.get() : renderer->getDefaultMaterial().get();
    if (!materialToUse || !currentScene || objectId == 0xffffffff)
        return;

    auto state = renderer->getState();
    AffectingLights lights = materialToUse->isLighted() ? currentScene->collectAffectingLights(getPosition(), 0.0f) : AffectingLights{};
    materialToUse->bindColor(objectId, lights, state->getViewProjectionMatrix() * getModelMatrix(), getModelMatrix(), getNormalMatrix(), mesh->getDataType());
    mesh->render(renderer->getFrameData());
}

void ActorSprite::setShadowRenderingMode(ShadowRenderingMode shadowRenderingMode)
{
    this->shadowRenderingMode = shadowRenderingMode;
}

void ActorSprite::setFrameSize(float frameWidth, float frameHeight)
{
    this->frameWidth = frameWidth;
    this->frameHeight = frameHeight;
}

void ActorSprite::setFrame(uint frame)
{
    this->frame = frame;
}

Actor::RenderPass ActorSprite::getRenderPass()
{
    return (!material || material->getColorBlending() == ColorBlending::Solid) ? RenderPass::Main : RenderPass::Blended;
}

float ActorSprite::getBoundingRadius()
{
    float size = getHighestAxisValue(scale);
    return sqrtf(size * size + size * size);
}
