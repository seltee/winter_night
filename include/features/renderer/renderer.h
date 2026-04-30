#pragma once
#include "features/data/model.h"
#include "features/renderer/mesh.h"
#include "features/scene/scene.h"
#include "core/core.h"
#include <vector>

namespace WNE
{
    class WNE_API Renderer
    {
    public:
        virtual ~Renderer();
        virtual void render();

        virtual void changeWindowSize(uint32 width, uint32 height);
        virtual std::shared_ptr<Mesh> createMesh(std::shared_ptr<Model> model);
        virtual void *getFrameData();

        void addScene(std::shared_ptr<Scene> scene);
    protected:
        std::vector<std::shared_ptr<Scene>> scenes;
    };
}