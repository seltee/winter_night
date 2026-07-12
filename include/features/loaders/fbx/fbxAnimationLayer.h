#pragma once
#include "core/core.h"
#include "features/data/animationTarget.h"
#include "fbxNode.h"
#include <memory>

namespace wne
{
    class FBXAnimationCurveNode;
    class FBXAnimationCurve;
    class Animation3d;

    class FBXAnimationLayer
    {
    public:
        struct AnimationBinding
        {
            AnimationTarget::KeyTranform keyTransform;
            std::string modelName;
        };

        FBXAnimationLayer(FBXNode &node);

        void linkAnimationCurveNode(FBXAnimationCurveNode *animationCurveNode);

        std::vector<uint64> getTimestampsList();

        void collectAnimationTargets(std::shared_ptr<Animation3d> animation, float timestamp);

        inline const uint64 getId() const
        {
            return id;
        }

        inline const std::string &getName() const
        {
            return name;
        }

    protected:
        float getCurveLerped(FBXAnimationCurve *curve, float time);

        uint64 id = 0;
        std::string name;

        std::vector<FBXAnimationCurveNode *> animationCurveNodes;
    };
};
