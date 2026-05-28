#include "features/scene/actor.h"
#include "features/scene/actorCamera.h"
#include "features/renderer/renderer.h"
#include "features/logger/logger.h"
#include <memory>

using namespace wne;

Actor::Actor(Renderer *renderer)
{
    this->renderer = renderer;
    name = "Actor";
}

void Actor::update(float delta)
{
    eventUpdate(delta);
}

void Actor::updateRenderFlag(ActorCamera *camera)
{
    if (camera)
    {
        Vector4 position = getModelMatrix() * Vector4(0.0f, 0.0f, 0.0f, 1.0f);
        position = position / position.w;
        isRenderedFlag = camera->checkFrustrum(position.xyz(), getBoundingRadius());
    }
    else
    {
        isRenderedFlag = false;
    }
}

void Actor::renderDepthShadow()
{
}

void Actor::renderDepth()
{
}

void Actor::renderColor()
{
}

void Actor::modelMatrixChanged()
{
    isDirtyNormalsFlag = true;
}

const Matrix3x3 &Actor::getNormalMatrix()
{
    auto mModel = getModelMatrix();
    if (isDirtyNormalsFlag)
    {
        mNormal = transpose(inverse(Matrix3x3(mModel)));
        isDirtyNormalsFlag = false;
    }
    return mNormal;
}

Actor::RenderPass Actor::getRenderPass()
{
    return RenderPass::Main;
}

void Actor::setScene(Scene *scene)
{
    if (currentScene && scene && (scene->getRenderer() != currentScene->getRenderer()))
    {
        Logger::log << "Error: you can't change actor's scene to a scene with a different renderer" << endl;
        return;
    }
    Scene *oldScene = this->currentScene;
    this->currentScene = scene;
    eventSetScene(oldScene, this->currentScene);
}

void Actor::eventCreated()
{
}

void Actor::eventSetScene(Scene *oldScene, Scene *newScene)
{
}

void Actor::eventUpdate(float delta)
{
}

void Actor::eventDestroyed()
{
}

float Actor::getBoundingRadius()
{
    return 1.0f;
}
