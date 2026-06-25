#pragma once

#include "features/ui/uiNode.h"
#include "core/core.h"
#include "core/math.h"
#include <memory>

namespace wne
{
    class Mesh;
    class Material;
    class Texture;

    class WNE_API UINodeImage : public UINode
    {
    public:
        UINodeImage(const UINodeImage &) = delete;
        UINodeImage &operator=(const UINodeImage &) = delete;

        UINodeImage(std::shared_ptr<Texture> image);
        UINodeImage(std::shared_ptr<Texture> image, uint width, uint height);

        static std::shared_ptr<UINodeImage> create(std::shared_ptr<Texture> image);
        static std::shared_ptr<UINodeImage> create(std::shared_ptr<Texture> image, uint width, uint height);

        ContextTreeNode update(const ContextUpdate &context) override;
        void render(const ContextRender &context) override;

        uint getWidth() override;
        uint getHeight() override;


    protected:
        bool isDirtyFlag = true;

        uint width = 0;
        uint height = 0;

        std::shared_ptr<Texture> image;

        std::shared_ptr<Material> material;
        std::shared_ptr<Mesh> mesh;
        Matrix4x4 mModel = Matrix4x4::identity();
        uint64 objectId = 0xffffffff;
        Renderer *renderer = nullptr;
    };
};