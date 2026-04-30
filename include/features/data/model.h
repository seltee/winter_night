/*
    Model holds raw object data in Ram
*/

#pragma once
#include <vector>
#include <memory>
#include "core/glm/glm.hpp"
#include "core/api.h"
#include "core/data.h"

namespace WNE
{
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
        ModelData data;
        ModelDataType dataType;

    public:
        Model(ModelData data, ModelDataType type);
        ~Model();

        static std::shared_ptr<Model> createFromData(const std::vector<VertexColored> &vertexColored);

        ModelDataType getDataType()
        {
            return dataType;
        }

        std::vector<VertexColored> &getAsVertexColored()
        {
            return *data.vertexColored;
        }
    };

}