#include "features/data/model.h"
#include <iostream>

using namespace WNE;

Model::Model(ModelData data, ModelDataType type)
{
    this->dataType = type;
    if (type == ModelDataType::VertexColored)
    {
        this->data.vertexColored = data.vertexColored;
    }
}

Model::~Model()
{
    if (dataType == ModelDataType::VertexColored)
    {
        delete data.vertexColored;
    }
}

std::shared_ptr<Model> Model::createFromData(const std::vector<VertexColored> &vertexColored)
{
    ModelData data;
    data.vertexColored = new std::vector<VertexColored>(vertexColored);
    return std::make_shared<Model>(data, ModelDataType::VertexColored);
}