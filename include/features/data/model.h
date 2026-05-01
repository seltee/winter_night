/*
    Model holds raw object data in Ram
*/

#pragma once
#include <vector>
#include <memory>
#include "core/api.h"
#include "core/data.h"
#include "core/core.h"

namespace wne
{
    enum class ModelDataType
    {
        VertexColoredInd16,
        VertexColoredInd32
    };

    union ModelVertexData
    {
        std::vector<VertexColored> *vertexColored;
    };
    union ModelIndexData
    {
        std::vector<uint16> *ind16;
        std::vector<uint32> *ind32;
    };

    class WNE_API Model
    {
    protected:
        ModelVertexData vertexData;
        ModelIndexData indexData;
        ModelDataType dataType;

    public:
        Model(ModelVertexData vertexData, ModelIndexData indexData, ModelDataType type);
        ~Model();

        static std::shared_ptr<Model> createFromData(const std::vector<VertexColored> &vertexColored, const std::vector<uint16> &indices);
        static std::shared_ptr<Model> createFromData(const std::vector<VertexColored> &vertexColored, const std::vector<uint32> &indices);

        inline bool is32bitIndicides()
        {
            return dataType == ModelDataType::VertexColoredInd32;
        }

        inline ModelDataType getDataType()
        {
            return dataType;
        }

        inline std::vector<VertexColored> &getAsVertexColored()
        {
            return *vertexData.vertexColored;
        }

        inline std::vector<uint16> &getAsIndex16()
        {
            return *indexData.ind16;
        }

        inline std::vector<uint32> &getAsIndex32()
        {
            return *indexData.ind32;
        }
    };

}