#pragma once
#include "features/scene/object.h"
#include "features/renderer/mesh.h"
#include "core/api.h"
#include <memory>

namespace wne
{
    class WNE_API ObjectMesh : public Object
    {
    public:
        ObjectMesh(std::shared_ptr<Mesh> mesh);
        static std::shared_ptr<ObjectMesh> create(std::shared_ptr<Mesh> mesh);

        void render(Renderer *renderer) override final;

    protected:
        std::shared_ptr<Mesh> mesh;
    };
};
