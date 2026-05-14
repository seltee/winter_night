#include "features/data/lightDirectional.h"

using namespace wne;

LightDirectional::LightDirectional() : Light(Light::Type::Directional)
{
}

bool LightDirectional::enableShadows(uint amountOfCascades, uint resolition)
{
    return true;
}

void LightDirectional::disableShadows()
{
}
