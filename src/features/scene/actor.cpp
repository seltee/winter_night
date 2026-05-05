#include "features/scene/actor.h"
#include "features/renderer/renderer.h"

using namespace wne;

Actor::Actor()
{
    name = "Actor";
}

void Actor::update(float delta)
{
}

void Actor::render(Renderer *renderer)
{
}

const Matrix4x4 &Actor::getModelMatrix()
{
    if (isDirtyFlag)
    {
        isDirtyFlag = false;
        Matrix4x4 newModel = Matrix4x4::translation(position);
        newModel = newModel * asMatrix(rotation);
        mModel = newModel * Matrix4x4::scale(scale);
    }
    return mModel;
}

Matrix4x4 Actor::getInvModelMatrix()
{
    return inverse(mModel);
}
