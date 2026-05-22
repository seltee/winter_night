#include "features/loaders/fbx/fbx.h"
#include "features/loaders/fbx/fbxHeader.h"
#include "features/loaders/fbx/fbxGlobalSettings.h"
#include <iostream>

using namespace wne;

FBX::FBX()
{
}

std::shared_ptr<Base3d> FBX::loadFile(const char *path)
{
    // open file
    std::shared_ptr<Base3d> base = std::make_shared<Base3d>();
    auto file = File::openBinaryToRead(path);
    if (!file->isValid())
        return nullptr;

    // check header
    auto header = FBXHeader::readHeader(file);
    if (!header || !header->isValid())
        return nullptr;

    auto nodes = FBXNode::readTree(*file);
    auto objects = findByName(nodes, "Objects");
    auto connections = findByName(nodes, "Connections");
    auto globalSettings = findByName(nodes, "GlobalSettings");

    if (!objects || !connections || !globalSettings)
        return nullptr;

    // auto fbxGlobalSettings = std::make_unique<FBXGlobalSettings>(*globalSettings);

    std::vector<FBXGeometry> geometries;
    std::vector<FBXModel> models;

    for (auto &geometryNode : objects->getChildrenByName("Geometry"))
        geometries.emplace_back(FBXGeometry(*geometryNode));
    for (auto &geometryNode : objects->getChildrenByName("Model"))
        models.emplace_back(FBXModel(*geometryNode));

    // auto animationStackNodes = objects->getChildrenByName("AnimationStack");
    // auto animationLayerNodes = objects->getChildrenByName("AnimationLayer");
    // auto animationCurveNodeNodes = objects->getChildrenByName("AnimationCurveNode");
    // auto animationCurveNodes = objects->getChildrenByName("AnimationCurve");
    // auto nodeAttributeNodes = objects->getChildrenByName("NodeAttribute");
    // auto poseNodes = objects->getChildrenByName("Pose");
    // auto deformerNodes = objects->getChildrenByName("Deformer");

    // Connecting models, geometry and animations
    for (auto &connection : connections->getChildren())
    {
        unsigned long long indexFrom = connection->getLong(1);
        unsigned long long indexTo = connection->getLong(2);

        FBXModel *modelFrom = getModelById(models, indexFrom);
        if (modelFrom)
        {
            if (indexTo)
            {
                FBXModel *modelTo = getModelById(models, indexTo);
                if (modelTo)
                    modelFrom->setParent(modelTo);
            }
            continue;
        }

        FBXGeometry *geometryFrom = getGeometryById(geometries, indexFrom);
        if (geometryFrom)
        {
            FBXModel *modelTo = getModelById(models, indexTo);
            if (modelTo)
                modelTo->setGeometry(geometryFrom);
            continue;
        }

        std::cout << "Unknown link " << indexFrom << " to " << indexTo << std::endl;
    }

    for (auto &model : models)
    {
        Matrix4x4 translation = Matrix4x4::translation(model.getPosition());
        Matrix4x4 rotationX = Matrix4x4::rotationX(model.getRotation().x);
        Matrix4x4 rotationY = Matrix4x4::rotationY(model.getRotation().y);
        Matrix4x4 rotationZ = Matrix4x4::rotationZ(model.getRotation().z);
        Matrix4x4 scale = Matrix4x4::scale(model.getScale());

        Matrix4x4 transformation = translation;
        transformation = transformation * rotationX;
        transformation = transformation * rotationY;
        transformation = transformation * rotationZ;
        transformation = transformation * scale;

        base->addModel(model.getName(), model.getAsModel(), transformation);
    }

    return base;
}

FBXNode *FBX::findByName(std::vector<std::unique_ptr<wne::FBXNode>> &list, const char *name)
{
    for (auto &it : list)
    {
        if (it->isName(name))
            return it.get();
    }
    return nullptr;
}

FBXGeometry *FBX::getGeometryById(std::vector<FBXGeometry> &list, uint64 id)
{
    for (auto &it : list)
    {
        if (it.getId() == id)
            return &it;
    }
    return nullptr;
}

FBXModel *FBX::getModelById(std::vector<FBXModel> &list, uint64 id)
{
    for (auto &it : list)
    {
        if (it.getId() == id)
            return &it;
    }
    return nullptr;
}