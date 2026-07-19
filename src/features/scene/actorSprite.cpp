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
    updateUV();
}

void ActorSprite::setMaterial(std::shared_ptr<Material> material)
{
    this->material = std::move(material);
}

void ActorSprite::renderDepthShadow(Vector3 &lightPosition)
{
    Material *materialToUse = material ? material.get() : renderer->getDefaultMaterial().get();
    if (!materialToUse || !currentScene || objectId == 0xffffffff)
        return;

    auto state = renderer->getState();
    if (shadowRenderingMode == ShadowRenderingMode::FromCamera)
    {
        materialToUse->bindDepthShadow(objectId, renderer, state->getViewProjectionMatrix() * getModelMatrix(), getNormalMatrix(), uvModifier, nullptr, true, mesh->getDataType());
    }
    else if (shadowRenderingMode == ShadowRenderingMode::FromLight)
    {
        Matrix4x4 mShadowModel;
        if (parent)
        {
            Matrix4x4 translation = Matrix4x4::translation(position);
            Matrix4x4 mPosition = parent->getModelMatrix() * translation;
            Vector4 absolutePosition = mPosition * Vector4(0, 0, 0, 1.0f);
            absolutePosition = absolutePosition / absolutePosition.w;

            Matrix4x4 newShadowModel = Matrix4x4::translation(absolutePosition);
            newShadowModel = newShadowModel * Matrix4x4(lookAt(absolutePosition.xyz(), state->getCameraPosition()));
            mShadowModel = newShadowModel * Matrix4x4::scale(scale);
        }
        else
        {
            Matrix4x4 newShadowModel = Matrix4x4::translation(position);
            newShadowModel = newShadowModel * Matrix4x4(lookAt(position, lightPosition));
            mShadowModel = newShadowModel * Matrix4x4::scale(scale);
        }

        materialToUse->bindDepthShadow(objectId, renderer, state->getViewProjectionMatrix() * mShadowModel, getNormalMatrix(), uvModifier, nullptr, false, mesh->getDataType());
    }
    mesh->render(renderer->getFrameData());
}

void ActorSprite::renderDepth()
{
    Material *materialToUse = material ? material.get() : renderer->getDefaultMaterial().get();
    if (!materialToUse || materialToUse->getColorBlending() != ColorBlending::Solid || !currentScene || objectId == 0xffffffff)
        return;

    auto state = renderer->getState();
    materialToUse->bindDepth(objectId, state->getViewProjectionMatrix() * getModelMatrix(), getModelMatrix(), getNormalMatrix(), uvModifier, nullptr, mesh->getDataType());
    mesh->render(renderer->getFrameData());
}

void ActorSprite::renderColor()
{
    Material *materialToUse = material ? material.get() : renderer->getDefaultMaterial().get();
    if (!materialToUse || !currentScene || objectId == 0xffffffff)
        return;

    auto state = renderer->getState();
    AffectingLights lights = materialToUse->isLighted() ? currentScene->collectAffectingLights(getPosition(), 0.0f) : AffectingLights{};
    materialToUse->bindColor(objectId, lights, state->getViewProjectionMatrix() * getModelMatrix(), getModelMatrix(), getNormalMatrix(), uvModifier, nullptr, mesh->getDataType());
    mesh->render(renderer->getFrameData());
}

void ActorSprite::setFrame(uint frame)
{
    this->frame = frame;
}

void ActorSprite::updateUV()
{
    uint32 frameX = frame % framesHorizontal;
    uint32 frameY = frame / framesHorizontal;

    uvModifier = {
        static_cast<float>(frameX) / static_cast<float>(framesHorizontal),
        static_cast<float>(frameY) / static_cast<float>(framesVertical),
        1.0f / static_cast<float>(framesHorizontal),
        1.0f / static_cast<float>(framesVertical)};
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
