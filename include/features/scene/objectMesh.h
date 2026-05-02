#pragma once
#include "features/scene/object.h"
#include "features/renderer/mesh.h"
#include "core/api.h"
#include "core/math.h"
#include <memory>

namespace wne
{
    class WNE_API ObjectMesh : public Object
    {
    public:
        ObjectMesh(std::shared_ptr<Mesh> mesh);
        static std::shared_ptr<ObjectMesh> create(std::shared_ptr<Mesh> mesh);

        void update(float delta) override final;
        void render(Renderer *renderer) override final;

    protected:
        std::shared_ptr<Mesh> mesh;
        Matrix4x4 mModel;

        float r = 0.0f;
    };
};
