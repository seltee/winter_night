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
            baseModel->append(model.model.get());
        }
        return baseModel;
    }
    return nullptr;
}

std::vector<std::shared_ptr<Model>> Base3d::getModels()
{
    std::vector<std::shared_ptr<Model>> modelsList;
    for (auto &model : models)
    {
        modelsList.push_back(model.model);
    }
    return modelsList;
}

std::vector<std::shared_ptr<Animation3d>> Base3d::getAnimations()
{
    std::vector<std::shared_ptr<Animation3d>> animationsList;
    for (auto &animation : animations)
    {
        animationsList.push_back(animation.animation);
    }
    return animationsList;
}

BaseModel *Base3d::addModel(const char *name, std::shared_ptr<Model> model)
{
    models.push_back({std::string(name), std::move(model)});
    return &models[models.size() - 1];
}

BaseAnimation *Base3d::addAnimation(const char *name, std::shared_ptr<Animation3d> animation)
{
    animations.push_back({std::string(name), std::move(animation)});
    return &animations[animations.size() - 1];
}