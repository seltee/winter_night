#pragma once
#include "core/math.h"
#include "core/api.h"
namespace wne
{
    class WNE_API Renderer;

    class Object
    {
    public:
        virtual void update(float delta);
        virtual void render(Renderer *renderer);
        virtual const Matrix4x4 &getModelMatrix();
        virtual Matrix4x4 getInvModelMatrix();

    protected:
        Matrix4x4 mModel = Matrix4x4::identity();
    };
}