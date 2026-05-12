#include "features/scene/light.h"

using namespace wne;

Light::Light(Type type)
{
    this->type = type;
}

void Light::prepareForRender()
{
}