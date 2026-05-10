#include "features/loaders/fbx/fbxModel.h"
#include <iostream>

using namespace wne;

FBXModel::FBXModel(FBXNode &node)
{
    id = node.getLong(0);
    name = node.getString(1);

    auto propertiesNode = node.findNode("Properties70");
    if (!propertiesNode)
        return;

    for (auto &childNode : propertiesNode->getChildren())
    {
        uint32 entryIndex = 0;
        char transformationType = 'n';
        Vector3 out(0, 0, 0);
        for (auto &property : childNode->getProperties())
        {
            if (property.isString() && strcmp(property.asString(), "Lcl Translation") == 0)
                transformationType = 'p';
            if (property.isString() && strcmp(property.asString(), "Lcl Rotation") == 0)
                transformationType = 'r';
            if (property.isString() && strcmp(property.asString(), "Lcl Scaling") == 0)
                transformationType = 's';
            if (!property.isString() && entryIndex < 3)
            {
                out[entryIndex] = property.asFloat();
                entryIndex++;
            }
        }

        if (transformationType == 'p')
            position = out;
        if (transformationType == 's')
            scale = out;
        if (transformationType == 'r')
            rotation = out * PI / 180.0f;
    }
}

std::shared_ptr<Model> FBXModel::getAsModel()
{
    if (geometry)
    {
        std::vector<VertexTextured> vertexTextured;
        std::vector<uint32> indices;

        geometry->getData(vertexTextured, indices);

        return Model::createFromData(vertexTextured, indices);
    }
    else
    {
        return std::shared_ptr<Model>(nullptr);
    }
}
