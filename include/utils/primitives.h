#pragma once
#include "core/math.h"
#include "core/api.h"
#include "features/data/model.h"
#include <memory>

namespace wne
{
    class WNE_API Primitives
    {
    public:
        static std::shared_ptr<Model> createPlain(float halfSize = 0.5f);
        static std::shared_ptr<Model> createPlain(float halfSize, const Vector3 &normal);
        static std::shared_ptr<Model> createBox(float size);
        static std::shared_ptr<Model> createBox(const Vector3 &size);
        static std::shared_ptr<Model> createSphere(float radius, unsigned int rings, unsigned int segments);

    private:
        Primitives();
    };
};
