#pragma once
#include <memory>
#include <vector>
#include "features/scene/object.h"
#include "features/scene/actor.h"
#include "features/scene/actorCamera.h"
#include "features/renderer/material.h"
#include "core/api.h"
#include "core/data.h"
#include "core/core.h"
#include "core/api.h"

namespace wne
{
    class Renderer;
    class Light;

    class WNE_API Scene
    {
    public:
        static std::shared_ptr<Scene> create();

        void update(float delta);
        void renderShadows(Renderer *renderer);
        void renderDepthShadow(Renderer *renderer);
        void renderDepth(Renderer *renderer);
        void render(Renderer *renderer);
        void calcSceneMVP();
        void provideSceneMVP(Renderer *renderer);
        void provideSceneData(Renderer *renderer);

        void addActor(std::shared_ptr<Actor> actor);
        void setCamera(std::shared_ptr<ActorCamera> actorCamera);

        void setAtmosphere(std::shared_ptr<Texture> atmosphereMap, std::shared_ptr<Texture> atmosphereRadiance, float atmosphereRadianceFactor = 1.0f);

        AffectingLights collectAffectingLights();

        // called when actor with light added
        // actor owns and removes light so it's ok to be just a pointer
        void registerLight(Light *light);

        // called when actor with light removed
        void unregisterLight(Light *light);

        inline void setAmbientLight(float r, float g, float b, float a = 1.0f)
        {
            ambientLightColor = Vector4(r, g, b, a);
        }

        inline void setAmbientLight(Vector4 color)
        {
            ambientLightColor = color;
        }

        inline Vector4 getAmbientLight()
        {
            return ambientLightColor;
        }

        inline std::shared_ptr<ActorCamera> getCameraActor()
        {
            return actorCamera;
        }

    protected:
        std::vector<std::shared_ptr<Actor>> actors;
        std::vector<Light *> lights;
        std::shared_ptr<ActorCamera> actorCamera;

        std::shared_ptr<Texture> atmosphereMap;
        std::shared_ptr<Texture> atmosphereRadiance;
        float atmosphereRadianceFactor = 1.0f;
        std::shared_ptr<Material> atmoMaterial;

        Vector4 ambientLightColor = {0.8f, 0.8f, 0.8f, 1.0f};

        Matrix4x4 mVP = Matrix4x4::identity();
    };
};
