#include "features/scene/actor.h"
#include "features/renderer/renderer.h"
#include <memory>

using namespace wne;

Actor::Actor()
{
    name = "Actor";
}

void Actor::update(float delta)
{
    eventUpdate(delta);
}

void Actor::renderDepthShadow(Renderer *renderer)
{
}

void Actor::renderDepth(Renderer *renderer)
{
}

void Actor::renderColor(Renderer *renderer)
{
}

const Matrix4x4 &Actor::getModelMatrix()
{
    if (isDirtyFlag)
    {
        isDirtyFlag = false;
        isDirtyNormalsFlag = true;
        Matrix4x4 newModel = Matrix4x4::translation(position);
        newModel = newModel * asMatrix(rotation);
        mModel = newModel * Matrix4x4::scale(scale);
    }
    return mModel;
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

Matrix4x4 Actor::getInvModelMatrix()
{
    return inverse(getModelMatrix());
}

Actor::RenderPass Actor::getRenderPass()
{
    return RenderPass::Main;
}

void Actor::setScene(Scene *scene)
{
    Scene *oldScene = this->currentScene;
    this->currentScene = scene;
    eventSetScene(oldScene, this->currentScene);
}

void Actor::eventSetScene(Scene *oldScene, Scene *newScene)
{
}

void Actor::eventUpdate(float delta)
{
}