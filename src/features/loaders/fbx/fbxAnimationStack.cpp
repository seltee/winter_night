#include "features/loaders/fbx/fbxAnimationStack.h"
#include "features/logger/logger.h"

using namespace wne;

FBXAnimationStack::FBXAnimationStack(FBXNode &node)
{
    id = node.getLong(0);
    auto propertiesNode = node.findNode("Properties70");

    if (!propertiesNode)
        return;

    for (auto &childNode : propertiesNode->getChildren())
    {
        std::string type = "nan";
        uint64 longValue = 0;
        for (auto &property : childNode->getProperties())
        {
            if (property.isString())
            {
                Logger::log << property.asString() << endl;
                if (!strcmp(property.asString(), "LocalStop"))
                    type = "lstop";
                if (!strcmp(property.asString(), "ReferenceStop"))
                    type = "rstop";
            }
            if (!property.isString())
            {
                longValue = property.asLong();
                break;
            }
        }

        if (type == "lstop")
            localTime = longValue;
        else if (type == "rstop")
            referenceTime = longValue;
    }

    Logger::log << "Animation stack " << id << ", " << localTime << " " << referenceTime << endl;
}