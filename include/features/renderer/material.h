#pragma once
#include "features/renderer/texture.h"
#include "features/data/model.h"
#include "core/math.h"
#include "core/api.h"
#include <memory>

namespace wne
{
    class WNE_API Material
    {
    public:
        virtual void rebuild();
        virtual void bind(const Matrix4x4 &mMVP, ModelDataType dataType);
    };
};