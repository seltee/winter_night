#pragma once
#include "features/loaders/loader3d.h"
#include "features/data/model.h"
#include "core/core.h"
#include "core/api.h"
#include <memory>

namespace wne
{
    class WNE_API Loader3d
    {
    public:
        static std::shared_ptr<Model> loadAsModel(const char *path);

    protected:
        Loader3d();
    };
};