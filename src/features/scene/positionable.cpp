#include "features/scene/positionable.h"
#include <iostream>

using namespace wne;

const Matrix4x4 &Positionable::getModelMatrix()
{
    if (isDirtyFlag)
    {
        isDirtyFlag = false;
        Matrix4x4 newModel = Matrix4x4::translation(position);
        newModel = newModel * asMatrix(rotation);
        mModel = newModel * Matrix4x4::scale(scale);
        if (parent)
            mModel = parent->getModelMatrix() * mModel;
        modelMatrixChanged();
    }
    return mModel;
}

Matrix4x4 Positionable::getInvModelMatrix()
{
    return inverse(getModelMatrix());
}

void Positionable::modelMatrixChanged()
{
}