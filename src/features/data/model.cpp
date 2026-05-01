#include "features/data/model.h"
#include <iostream>

using namespace wne;

Model::Model(ModelVertexData vertexData, ModelIndexData indexData, ModelDataType type)
{
    this->dataType = type;
    if (type == ModelDataType::VertexColoredInd16 || type == ModelDataType::VertexColoredInd32)
    {
        this->vertexData.vertexColored = vertexData.vertexColored;
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
    if (is32bitIndicides())
    {
        delete indexData.ind32;
    }
    else
    {
        delete indexData.ind16;
    }
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
