#include "features/loaders/fbx/fbxGlobalSettings.h"
#include <iostream>

using namespace wne;

FBXGlobalSettings::FBXGlobalSettings(FBXNode &node)
{
    auto propertiesNode = node.findNode("Properties70");

    for (auto &childNode : propertiesNode->getChildren())
    {
        std::cout << childNode->getName() << std::endl;
        DataType type = DataType::None;

        for (auto &property : childNode->getProperties())
        {
            if (property.isString())
                std::cout << "- " << property.asString() << std::endl;
            if (property.isString() && strcmp(property.asString(), "UnitScaleFactor") == 0)
                type = DataType::UnitScaleFactor;

            if (!property.isString())
            {
                std::cout << property.asFloat() << " " << property.type << std::endl;
            }
        }
    }
}