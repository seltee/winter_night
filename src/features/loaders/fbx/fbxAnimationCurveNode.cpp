#include "features/loaders/fbx/fbxAnimationCurveNode.h"
#include "features/loaders/fbx/fbxModel.h"
#include "features/logger/logger.h"

using namespace wne;

FBXAnimationCurveNode::FBXAnimationCurveNode(FBXNode &node)
{
    id = node.getLong(0);
    const char *nodeType = node.getString(1);

    if (!nodeType)
        return;

    if (!strcmp(nodeType, "S"))
        type = "scl";
    if (!strcmp(nodeType, "T"))
        type = "pos";
    if (!strcmp(nodeType, "R"))
        type = "rot";

    auto propertiesNode = node.findNode("Properties70");
    for (auto &childNode : propertiesNode->getChildren())
    {
        std::string dataType = "n";
        for (auto &property : childNode->getProperties())
        {
            if (property.isString())
            {
                if (strcmp(property.asString(), "d|X") == 0)
                    dataType = "x";
                if (strcmp(property.asString(), "d|Y") == 0)
                    dataType = "y";
                if (strcmp(property.asString(), "d|Z") == 0)
                    dataType = "z";
            }
            if (property.isDouble() || property.isFloat())
            {
                if (dataType == "x")
                    defaultValue.x = property.asFloat();
                if (dataType == "y")
                    defaultValue.y = property.asFloat();
                if (dataType == "z")
                    defaultValue.z = property.asFloat();
            }
        }
    }

    // Logger::log << "FBX Animation Curve Node " << nodeType << " " << type << endl;
    // Logger::log << "Default value " << defaultValue.x << " " << defaultValue.y << " " << defaultValue.z << endl;
}

void FBXAnimationCurveNode::linkCurve(FBXAnimationCurve *curve, FBXNode *node)
{
    for (auto &property : node->getProperties())
    {
        if (property.isString())
        {
            if (!strcmp(property.asString(), "d|X"))
                curves.push_back({curve, 'x'});
            if (!strcmp(property.asString(), "d|Y"))
                curves.push_back({curve, 'y'});
            if (!strcmp(property.asString(), "d|Z"))
                curves.push_back({curve, 'z'});
        }
    }
}

void FBXAnimationCurveNode::addAffectedModel(FBXModel *model)
{
    affectedModels.push_back(model);
}

FBXAnimationCurve *FBXAnimationCurveNode::getXCurve()
{
    for (auto &curve : curves)
    {
        if (curve.axis == 'x')
            return curve.curve;
    }
    return nullptr;
}

FBXAnimationCurve *FBXAnimationCurveNode::getYCurve()
{
    for (auto &curve : curves)
    {
        if (curve.axis == 'y')
            return curve.curve;
    }
    return nullptr;
}

FBXAnimationCurve *FBXAnimationCurveNode::getZCurve()
{
    for (auto &curve : curves)
    {
        if (curve.axis == 'z')
            return curve.curve;
    }
    return nullptr;
}

bool FBXAnimationCurveNode::hasModelName(std::string modelName)
{
    for (auto &model : affectedModels)
    {
        if (model->getName() == modelName)
            return true;
    }

    return false;
}
