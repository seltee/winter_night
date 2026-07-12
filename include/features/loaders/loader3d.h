#pragma once
#include "features/loaders/loader3d.h"
#include "features/data/animation3d.h"
#include "features/data/model.h"
#include "core/core.h"
#include "core/api.h"
#include <memory>
#include <vector>

namespace wne
{
    class WNE_API Loader3d
    {
    public:
        struct ModelAnimations
        {
            std::vector<std::shared_ptr<Model>> models;
            std::vector<std::shared_ptr<Animation3d>> animations;
        };

        static std::shared_ptr<Model> loadAsModel(const char *path);
        static std::vector<std::shared_ptr<Model>> loadAsModels(const char *path);
        static ModelAnimations loadAsModelAnimation(const char *path);

    protected:
        Loader3d();
    };
};