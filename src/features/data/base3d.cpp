#include "features/data/base3d.h"
#include <iostream>

using namespace wne;

std::shared_ptr<Model> Base3d::getAsModel()
{
    if (models.size() > 0)
    {
        std::shared_ptr<Model> baseModel = Model::create(ModelDataType::VertexTexturedInd32);
        for (auto &model : models)
        {
            if (model.model->getAsVertexTextured().size() == 0)
                continue;
            baseModel->append(model.model.get(), model.transform);
        }
        return baseModel;
    }
    return nullptr;
}

BaseModel *Base3d::addModel(const char *name, std::shared_ptr<Model> model)
{
    auto identity = Matrix4x4::identity();
    return addModel(name, std::move(model), identity);
}

BaseModel *Base3d::addModel(const char *name, std::shared_ptr<Model> model, Matrix4x4 &transformation)
{
    models.push_back({std::string(name), std::move(model), transformation});
    return &models[models.size() - 1];
}