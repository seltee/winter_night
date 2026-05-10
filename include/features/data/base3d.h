#pragma once
#include "features/data/model.h"
#include "core/math.h"
#include <memory>
#include <cstring>

namespace wne
{
    struct BaseModel
    {
        std::string name;
        std::shared_ptr<Model> model;
        Matrix4x4 transform;
    };

    class Base3d
    {
    public:
        std::shared_ptr<Model> getAsModel();

        BaseModel *addModel(const char *name, std::shared_ptr<Model> model);
        BaseModel *addModel(const char *name, std::shared_ptr<Model> model, Matrix4x4 &transformation);

    protected:
        std::vector<BaseModel> models;
    };
};