#pragma once
#include "core/glm/glm.hpp"

class ModelRender
{
protected:
    ModelRender();
    virtual ~ModelRender();

public:
    virtual void destroy();
};