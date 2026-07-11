#include "features/loaders/fbx/fbxAnimationCurve.h"
#include "features/logger/logger.h"

using namespace wne;

FBXAnimationCurve::FBXAnimationCurve(FBXNode &node)
{
    id = node.getLong(0);

    std::vector<uint64> keysTime;
    std::vector<float> keysValue;

    for (auto &childNode : node.getChildren())
    {
        if (childNode->isName("KeyTime"))
        {
            auto arr = childNode->getArrayLongs(0);
            uint size = childNode->getElementCount(0);
            keysTime.resize(size);
            for (uint i = 0; i < size; i++)
                keysTime[i] = arr[i];
        }
        else if (childNode->isName("KeyValueFloat"))
        {
            auto arr = childNode->getArrayFloats(0);
            uint size = childNode->getElementCount(0);
            keysValue.resize(size);
            for (uint i = 0; i < size; i++)
                keysValue[i] = arr[i];
        }
        else if (childNode->isName("Default"))
        {
            // Logger::log << "Key default" << endl;
        }
        else if (childNode->isName("KeyVer"))
        {
            // Logger::log << "Key version" << endl;
        }
        else if (childNode->isName("KeyAttrFlags"))
        {
            // Logger::log << "Key attr flags" << endl;
        }
        else if (childNode->isName("KeyAttrRefCount"))
        {
            auto arr = childNode->getArrayIntegers(0);
            uint size = childNode->getElementCount(0);
            keyAttrReference.clear();
            keyAttrReference.resize(size);
            for (uint i = 0; i < size; i++)
                keyAttrReference[i] = arr[i];
        }
        else if (childNode->isName("KeyAttrDataFloat"))
        {
            // Logger::log << "Key attr data float" << endl;
        }
        else
        {
            // Logger::log << "Unknown property " << childNode->getName() << endl;
        }
    }

    if (keysTime.size() != keysValue.size())
    {
        Logger::log << "FBX Inconsistent value and time vector sizes detected in FBXAnimationCurve " << keysTime.size() << " vs " << keysValue.size() << endl;
        return;
    }

    auto size = keysTime.size();
    keyTimeList.resize(size);
    for (uint64 i = 0; i < size; i++)
    {
        keyTimeList[i] = KeyTime({keysTime[i],
                                  keysValue[i]});
    }
}