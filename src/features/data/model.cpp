#include "features/data/model.h"

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

Model *Model::createFromData(std::vector<VertexColored> &vertexColored)
{
    ModelData data;
    data.vertexColored = new std::vector<VertexColored>(vertexColored);
    return new Model(data, ModelDataType::VertexColored);
}

void Model::destroy()
{
    delete this;
}