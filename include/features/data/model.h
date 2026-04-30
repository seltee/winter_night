#pragma once
#include "core/glm/glm.hpp"
#include "core/api.h"
#include "core/data.h"
#include <vector>

enum class ModelDataType
{
    VertexColored
};

union ModelData
{
    std::vector<VertexColored> *vertexColored;
};

class WNE_API Model
{
protected:
    Model(ModelData data, ModelDataType type);
    virtual ~Model();

    ModelData data;
    ModelDataType dataType;

public:
    static Model *createFromData(std::vector<VertexColored> &vertexColored);

    virtual void destroy();
};
