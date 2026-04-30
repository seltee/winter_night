#pragma once
#include "features/data/modelRender.h"
#include "features/data/model.h"

namespace WNE
{
    class VulkanModelRender : public ModelRender
    {
    protected:
        VulkanModelRender();
        ~VulkanModelRender();

    public:
        static VulkanModelRender *fromModel(Model *modelRender);
    };
}