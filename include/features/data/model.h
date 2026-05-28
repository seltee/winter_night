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
        Unknown,
        VertexColoredInd16,
        VertexColoredInd32,
        VertexTexturedInd16,
        VertexTexturedInd32
    };

    union ModelVertexData
    {
        std::vector<VertexColored> *vertexColored;
        std::vector<VertexTextured> *vertexTextured;
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
        float boundingRadius = 0.0f;

    public:
        Model(ModelVertexData vertexData, ModelIndexData indexData, ModelDataType type);
        ~Model();
        bool append(Model *model, Matrix4x4 &transformation);

        static std::shared_ptr<Model> create(ModelDataType type);
        static std::shared_ptr<Model> createFromData(const std::vector<VertexColored> &vertexColored, const std::vector<uint16> &indices);
        static std::shared_ptr<Model> createFromData(const std::vector<VertexColored> &vertexColored, const std::vector<uint32> &indices);
        static std::shared_ptr<Model> createFromData(const std::vector<VertexTextured> &vertexTextured, const std::vector<uint16> &indices);
        static std::shared_ptr<Model> createFromData(const std::vector<VertexTextured> &vertexTextured, const std::vector<uint32> &indices);

        inline bool is32bitIndicides()
        {
            return dataType == ModelDataType::VertexColoredInd32 || dataType == ModelDataType::VertexTexturedInd32;
        }

        inline ModelDataType getDataType()
        {
            return dataType;
        }

        inline std::vector<VertexColored> &getAsVertexColored()
        {
            return *vertexData.vertexColored;
        }

        inline std::vector<VertexTextured> &getAsVertexTextured()
        {
            return *vertexData.vertexTextured;
        }

        inline std::vector<uint16> &getAsIndex16()
        {
            return *indexData.ind16;
        }

        inline std::vector<uint32> &getAsIndex32()
        {
            return *indexData.ind32;
        }

        inline float getBoundingRadius()
        {
            return boundingRadius;
        }
    };

}