#pragma once
#include "core/api.h"
#include "core/math.h"
#include <string>

// Time in timestamp is 1.0 = 1000 ms or 1 second
// Due to float precision sweat spot

namespace wne
{

    class WNE_API AnimationTarget
    {
    public:
        struct KeyTranform
        {
            float timeStamp;
            Vector3 position;
            Vector3 rotation;
            Vector3 scale;
        };

        AnimationTarget(const std::string targetName);

        float getAnimationTimeLength();

        // interpolates between 2 frames
        Matrix4x4 getTransformByTime(float timeStamp);
        // picks static
        // void getTransformByTimeFixedFrame(float timeStamp, Entity *entity);

        inline bool isName(std::string targetName) { return this->targetName == targetName; }
        inline std::string getTargetName() { return targetName; }

        void addKey(KeyTranform keyTransform);

        inline uint getKeysCount()
        {
            return keys.size();
        }

    protected:
        std::string targetName;
        std::vector<KeyTranform> keys;
    };
}