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
        Matrix4x4 defaultTransformation = Matrix4x4::identity();
        std::string name = "NoNameModel";
        std::string parentName = "";

    public:
        Model(ModelVertexData vertexData, ModelIndexData indexData, ModelDataType type);
        ~Model();
        bool append(Model *model);

        static std::shared_ptr<Model> create(ModelDataType type);
        static std::shared_ptr<Model> createFromData(const std::vector<VertexColored> &vertexColored, const std::vector<uint16> &indices);
        static std::shared_ptr<Model> createFromData(const std::vector<VertexColored> &vertexColored, const std::vector<uint32> &indices);
        static std::shared_ptr<Model> createFromData(const std::vector<VertexTextured> &vertexTextured, const std::vector<uint16> &indices);
        static std::shared_ptr<Model> createFromData(const std::vector<VertexTextured> &vertexTextured, const std::vector<uint32> &indices);

        inline bool isEmpty()
        {
            if (is32bitIndicides())
                return indexData.ind32->size() == 0;
            else
                return indexData.ind16->size() == 0;
        }

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

        inline Matrix4x4 &getDefaultTransformation()
        {
            return defaultTransformation;
        }

        inline void setDefaultTransformation(const Matrix4x4 &defaultTransformation)
        {
            this->defaultTransformation = defaultTransformation;
        }

        inline const char *getName() const
        {
            return name.c_str();
        }

        inline void setName(const char *name)
        {
            this->name = std::string(name);
        }

        inline const char *getParentName() const
        {
            return parentName.c_str();
        }

        inline void setParentName(const char *parentName)
        {
            this->parentName = std::string(parentName);
        }
    };

}