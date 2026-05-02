#pragma once
#include "features/data/model.h"
#include "features/renderer/mesh.h"
#include "features/scene/scene.h"
#include "core/core.h"
#include "core/math.h"
#include <vector>

namespace wne
{
    class WNE_API Renderer
    {
    public:
        virtual ~Renderer();
        virtual void update(float delta);
        virtual void render();

        virtual void changeWindowSize(uint32 width, uint32 height);
        virtual std::shared_ptr<Mesh> createMesh(std::shared_ptr<Model> model);
        virtual void *getFrameData();

        virtual void setSyncState(bool syncEnabled);
        virtual bool getSyncState();

        virtual void setModelMatrix(Matrix4x4 &model);
        virtual void setViewProjectionMatrix(Matrix4x4 &VP);

        void addScene(std::shared_ptr<Scene> scene);
    protected:
        std::vector<std::shared_ptr<Scene>> scenes;
    };
}