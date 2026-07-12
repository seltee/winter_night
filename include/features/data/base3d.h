#pragma once
#include "features/data/model.h"
#include "features/data/animation3d.h"
#include "core/math.h"
#include <memory>
#include <cstring>

namespace wne
{
    struct BaseModel
    {
        std::string name;
        std::shared_ptr<Model> model;
    };

    struct BaseAnimation
    {
        std::string name;
        std::shared_ptr<Animation3d> animation;
    };

    class Base3d
    {
    public:
        std::shared_ptr<Model> getAsModel();
        std::vector<std::shared_ptr<Model>> getModels();
        std::vector<std::shared_ptr<Animation3d>> getAnimations();

        BaseModel *addModel(const char *name, std::shared_ptr<Model> model);

        BaseAnimation *addAnimation(const char *name, std::shared_ptr<Animation3d> animation);

    protected:
        std::vector<BaseModel> models;
        std::vector<BaseAnimation> animations;
    };
};