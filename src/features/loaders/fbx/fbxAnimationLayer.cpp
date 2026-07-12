#include "features/loaders/fbx/fbxAnimationLayer.h"
#include "features/loaders/fbx/fbxAnimationCurveNode.h"
#include "features/loaders/fbx/fbxAnimationCurve.h"
#include "features/loaders/fbx/fbxModel.h"
#include "features/logger/logger.h"
#include "core/math.h"
#include <algorithm>

using namespace wne;

const uint64 FBXTimeToMs = 46186158;

FBXAnimationLayer::FBXAnimationLayer(FBXNode &node)
{
    // node->print();
    id = node.getLong(0);
    const char *charName = node.getString(1);
    name = std::string(charName);
}

void FBXAnimationLayer::linkAnimationCurveNode(FBXAnimationCurveNode *animationCurveNode)
{
    animationCurveNodes.push_back(animationCurveNode);
}

std::vector<uint64> FBXAnimationLayer::getTimestampsList()
{
    std::vector<uint64> timestamps;
    for (auto &curveNode : animationCurveNodes)
    {
        for (auto &curve : curveNode->getCurves())
        {
            auto keys = curve.curve->getKeyTimeList();
            uint keysCount = keys.size();

            for (uint i = 0; i < keysCount; i++)
            {
                auto keyTime = keys[i].time;
                bool found = false;

                for (auto &time : timestamps)
                {
                    if (time == keyTime)
                        found = true;
                }
                if (!found)
                    timestamps.push_back(keyTime);
            }
        }
    }
    std::sort(timestamps.begin(), timestamps.end());
    return timestamps;
}

void FBXAnimationLayer::collectAnimationTargets(std::shared_ptr<Animation3d> animation, float timestamp)
{
    std::vector<AnimationBinding> animBindings;
    for (auto &curveNode : animationCurveNodes)
    {
        Vector3 out = curveNode->getDefaultValue();

        FBXAnimationCurve *curveX = curveNode->getXCurve();
        FBXAnimationCurve *curveY = curveNode->getYCurve();
        FBXAnimationCurve *curveZ = curveNode->getZCurve();

        if (curveX)
            out.x = getCurveLerped(curveX, timestamp);
        if (curveY)
            out.y = getCurveLerped(curveY, timestamp);
        if (curveZ)
            out.z = getCurveLerped(curveZ, timestamp);

        // Make sure we have all have names in the list of transforms
        for (auto &model : curveNode->getAffectedModels())
        {
            bool bFound = false;
            for (auto &binding : animBindings)
            {
                if (binding.modelName == model->getName())
                {
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
            {
                AnimationBinding binding;
                binding.modelName = model->getName();
                binding.keyTransform = AnimationTarget::KeyTranform({timestamp,
                                                                     model->getPosition(),
                                                                     model->getRotation(),
                                                                     model->getScale()});
                animBindings.push_back(binding);
            }
        }

        // affect transforms in list for this curve
        for (auto &anim : animBindings)
        {
            if (curveNode->hasModelName(anim.modelName))
            {
                if (curveNode->getType() == std::string("pos"))
                    anim.keyTransform.position = out;
                if (curveNode->getType() == std::string("scl"))
                    anim.keyTransform.scale = out;
                if (curveNode->getType() == std::string("rot"))
                    anim.keyTransform.rotation = (out / 180.0f) * PI;
            }
        }
    }

    // each timestamp we add transformation to anim bindings
    for (auto &animBinding : animBindings)
    {
        auto animTarget = animation->getAnimationTarget(animBinding.modelName, true);
        animTarget->addKey(animBinding.keyTransform);
    }
}

float FBXAnimationLayer::getCurveLerped(FBXAnimationCurve *curve, float time)
{
    auto keys = curve->getKeyTimeList();
    uint keysCount = keys.size();

    for (uint i = 0; i < keysCount; i++)
    {
        float keyTime = static_cast<float>(keys[i].time / FBXTimeToMs) / 1000.0f;
        float prevKeyTime = i > 0 ? static_cast<float>(keys[i - 1].time / FBXTimeToMs) / 1000.0f : 0.0f;

        if (time < keyTime)
        {
            if (i == 0)
            {
                return keys[i].value;
            }
            else
            {
                float cValue = keys[i].value;
                float pValue = keys[i - 1].value;
                float normalTime = (time - prevKeyTime) / (keyTime - prevKeyTime); // 0 - 1

                return pValue * (1.0f - normalTime) + cValue * normalTime;
            }
        }
    }

    return keys[keysCount - 1].value;
}