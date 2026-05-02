#pragma once
#include <memory>
#include <vector>
#include "features/scene/object.h"
#include "features/scene/objectCamera.h"
#include "core/api.h"
#include "core/data.h"
#include "core/core.h"
#include "core/api.h"

namespace wne
{
    class Renderer;

    class WNE_API Scene
    {
    public:
        static std::shared_ptr<Scene> create();

        void update(float delta);
        void render(Renderer *renderer);

        void addObject(std::shared_ptr<Object> object);
        void setCamera(std::shared_ptr<ObjectCamera> objectCamera);

    protected:
        std::vector<std::shared_ptr<Object>> objects;
        std::shared_ptr<ObjectCamera> objectCamera;
    };
};
