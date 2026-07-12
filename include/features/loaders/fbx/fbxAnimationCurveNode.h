#pragma once
#include "core/core.h"
#include "fbxNode.h"
#include <memory>

namespace wne
{
    class FBXModel;
    class FBXAnimationCurve;
    class FBXAnimationCurveNode
    {
    public:
        struct FBXCurveBinding
        {
            FBXAnimationCurve *curve;
            int8 axis;
        };

        FBXAnimationCurveNode(FBXNode &node);

        void linkCurve(FBXAnimationCurve *curve, FBXNode *node);
        void addAffectedModel(FBXModel *model);

        FBXAnimationCurve *getXCurve();
        FBXAnimationCurve *getYCurve();
        FBXAnimationCurve *getZCurve();

        bool hasModelName(std::string modelName);


        inline const std::vector<FBXCurveBinding> &getCurves() const
        {
            return curves;
        }

        inline const uint64 getId()
        {
            return id;
        }

        inline const std::string &getType() const
        {
            return type;
        }

        inline const Vector3 &getDefaultValue() const
        {
            return defaultValue;
        }

        inline std::vector<FBXModel *> &getAffectedModels()
        {
            return affectedModels;
        }

    private:
        uint64 id = 0;
        // none, scl, pos, rot
        std::string type = "none";
        Vector3 defaultValue{};

        std::vector<FBXCurveBinding> curves;
        std::vector<FBXModel *> affectedModels;
    };
};
