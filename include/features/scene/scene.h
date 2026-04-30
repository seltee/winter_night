#pragma once
#include <memory>
#include <vector>
#include "features/scene/object.h"
#include "core/api.h"
#include "core/data.h"
#include "core/core.h"
#include "core/api.h"

namespace WNE
{
    class Renderer;

    class WNE_API Scene
    {
    public:
        static std::shared_ptr<Scene> create();

        void render(Renderer *renderer);

        void addObject(std::shared_ptr<Object> object);

    protected:
        std::vector<std::shared_ptr<Object>> objects;
    };
};
