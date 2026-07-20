#include "features/data/model.h"
#include "features/logger/logger.h"
#include <iostream>

using namespace wne;

Model::Model(ModelVertexData vertexData, ModelIndexData indexData, ModelDataType type)
{
    this->dataType = type;
    if (type == ModelDataType::VertexColoredInd16 || type == ModelDataType::VertexColoredInd32)
    {
        this->vertexData.vertexColored = vertexData.vertexColored;
        for (auto const &vertex : *this->vertexData.vertexColored)
            boundingRadius = fmaxf(boundingRadius, sqrtf(vertex.pos.x * vertex.pos.x + vertex.pos.y * vertex.pos.y + vertex.pos.z * vertex.pos.z));
    }
    else if (type == ModelDataType::VertexTexturedInd16 || type == ModelDataType::VertexTexturedInd32)
    {
        this->vertexData.vertexTextured = vertexData.vertexTextured;
        for (auto const &vertex : *this->vertexData.vertexTextured)
            boundingRadius = fmaxf(boundingRadius, sqrtf(vertex.pos.x * vertex.pos.x + vertex.pos.y * vertex.pos.y + vertex.pos.z * vertex.pos.z));
    }
    if (is32bitIndicides())
        this->indexData.ind32 = indexData.ind32;
    else
        this->indexData.ind16 = indexData.ind16;
}

Model::~Model()
{
    if (dataType == ModelDataType::VertexColoredInd16 || dataType == ModelDataType::VertexColoredInd32)
    {
        delete vertexData.vertexColored;
    }
    else if (dataType == ModelDataType::VertexTexturedInd16 || dataType == ModelDataType::VertexTexturedInd32)
    {
        delete vertexData.vertexTextured;
    }
    if (is32bitIndicides())
    {
        delete indexData.ind32;
    }
    else
    {
        delete indexData.ind16;
    }
}

bool Model::append(Model *model)
{
    if (model->getDataType() != dataType || model->isEmpty())
        return false;

    Matrix4x4 transformation = model->getDefaultTransformation();

    Matrix3x3 normalMatrix = transpose(inverse(Matrix3x3(transformation)));

    uint32 indexShift = 0;
    if (dataType == ModelDataType::VertexColoredInd16 || dataType == ModelDataType::VertexColoredInd32)
    {
        indexShift = vertexData.vertexColored->size();
        uint32 vNum = indexShift;
        for (auto &vertex : model->getAsVertexColored())
        {
            Vector4 vec = transformation * Vector4(vertex.pos, 1.0f);
            vec = vec / vec.w;
            Vector3 normal = normalize(normalMatrix * vertex.normal);

            vertexData.vertexColored->emplace_back(
                VertexColored({vNum, {vec.x, vec.y, vec.z}, {vertex.color.r, vertex.color.g, vertex.color.b}, normal}));

            boundingRadius = fmaxf(boundingRadius, sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z));
            vNum++;
        }
    }
    else if (dataType == ModelDataType::VertexTexturedInd32 || dataType == ModelDataType::VertexTexturedInd16)
    {
        indexShift = vertexData.vertexTextured->size();
        uint32 vNum = indexShift;
        for (auto &vertex : model->getAsVertexTextured())
        {
            Vector4 vec = transformation * Vector4(vertex.pos, 1.0f);
            vec = vec / vec.w;
            Vector3 normal = normalize(normalMatrix * vertex.normal);

            vertexData.vertexTextured->emplace_back(
                VertexTextured({vNum, {vec.x, vec.y, vec.z}, {vertex.uv.x, vertex.uv.y}, normal}));

            boundingRadius = fmaxf(boundingRadius, sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z));
        }
    }

    if (is32bitIndicides())
    {
        for (auto &index : model->getAsIndex32())
            indexData.ind32->push_back(index + indexShift);
    }
    else
    {
        for (auto &index : model->getAsIndex16())
            indexData.ind16->push_back(index + indexShift);
    }

    return true;
}

std::shared_ptr<Model> Model::create(ModelDataType type)
{
    if (type == ModelDataType::VertexColoredInd16 || type == ModelDataType::VertexColoredInd32)
    {
        ModelVertexData vertexData;
        vertexData.vertexColored = new std::vector<VertexColored>();
        if (type == ModelDataType::VertexColoredInd16)
        {
            ModelIndexData indexData;
            indexData.ind16 = new std::vector<uint16>();
            return std::make_shared<Model>(vertexData, indexData, ModelDataType::VertexColoredInd16);
        }
        else
        {
            ModelIndexData indexData;
            indexData.ind32 = new std::vector<uint32>();
            return std::make_shared<Model>(vertexData, indexData, ModelDataType::VertexColoredInd32);
        }
    }
    else if (type == ModelDataType::VertexTexturedInd16 || type == ModelDataType::VertexTexturedInd32)
    {
        ModelVertexData vertexData;
        vertexData.vertexTextured = new std::vector<VertexTextured>();
        if (type == ModelDataType::VertexTexturedInd16)
        {
            ModelIndexData indexData;
            indexData.ind16 = new std::vector<uint16>();
            return std::make_shared<Model>(vertexData, indexData, ModelDataType::VertexTexturedInd16);
        }
        else
        {
            ModelIndexData indexData;
            indexData.ind32 = new std::vector<uint32>();
            return std::make_shared<Model>(vertexData, indexData, ModelDataType::VertexTexturedInd32);
        }
    }
    return nullptr;
}

std::shared_ptr<Model> Model::createFromData(const std::vector<VertexColored> &vertexColored, const std::vector<uint16> &indices)
{
    ModelVertexData vertexData;
    vertexData.vertexColored = new std::vector<VertexColored>(vertexColored);
    ModelIndexData indexData;
    indexData.ind16 = new std::vector<uint16>(indices);
    return std::make_shared<Model>(vertexData, indexData, ModelDataType::VertexColoredInd16);
}

std::shared_ptr<Model> Model::createFromData(const std::vector<VertexColored> &vertexColored, const std::vector<uint32> &indices)
{
    ModelVertexData vertexData;
    vertexData.vertexColored = new std::vector<VertexColored>(vertexColored);
    ModelIndexData indexData;
    indexData.ind32 = new std::vector<uint32>(indices);
    return std::make_shared<Model>(vertexData, indexData, ModelDataType::VertexColoredInd32);
}

std::shared_ptr<Model> Model::createFromData(const std::vector<VertexTextured> &vertexTextured, const std::vector<uint16> &indices)
{
    ModelVertexData vertexData;
    vertexData.vertexTextured = new std::vector<VertexTextured>(vertexTextured);
    ModelIndexData indexData;
    indexData.ind16 = new std::vector<uint16>(indices);
    return std::make_shared<Model>(vertexData, indexData, ModelDataType::VertexTexturedInd16);
}

std::shared_ptr<Model> Model::createFromData(const std::vector<VertexTextured> &vertexTextured, const std::vector<uint32> &indices)
{
    ModelVertexData vertexData;
    vertexData.vertexTextured = new std::vector<VertexTextured>(vertexTextured);
    ModelIndexData indexData;
    indexData.ind32 = new std::vector<uint32>(indices);
    return std::make_shared<Model>(vertexData, indexData, ModelDataType::VertexTexturedInd32);
}
