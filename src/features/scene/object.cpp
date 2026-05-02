#include "features/scene/object.h"
#include "features/renderer/renderer.h"

using namespace wne;

void Object::update(float delta)
{
}

void Object::render(Renderer *renderer)
{
}

const Matrix4x4 &Object::getModelMatrix()
{
    return mModel;
}

Matrix4x4 Object::getInvModelMatrix()
{
    return inverse(mModel);
}