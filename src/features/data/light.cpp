#include "features/data/light.h"
#include <iostream>

using namespace wne;

Light::Light(Type type)
{
    this->type = type;
}

void Light::updateTransformation(const Matrix4x4 &mModel)
{
    position = mModel * Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    position = position / position.w;

    Vector4 positionInDirection = mModel * direction;
    realDirection = Vector4(normalize(positionInDirection.xyz() - position.xyz()), 1.0f);
}

void Light::renderShadows(Scene *scene, ActorCamera *camera)
{
}

void Light::prepareForRender()
{
}
