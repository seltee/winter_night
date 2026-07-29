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
        Scene(Renderer *renderer);
        static std::shared_ptr<Scene> create(Renderer *renderer);

        void update(float delta);
        void renderShadows();
        void renderDepthShadow(Vector3 &lightPosition);
        void renderDepth();
        void render();
        void calcSceneMVP();
        void provideSceneMVP();
        void provideSceneData();

        void setCamera(std::shared_ptr<ActorCamera> actorCamera);

        void setAtmosphere(std::shared_ptr<Texture> atmosphereMap, std::shared_ptr<Texture> atmosphereRadiance, float atmosphereRadianceFactor = 1.0f);

        AffectingLights collectAffectingLights(const Vector3 &point, float radius);

        // called when actor with light added
        // actor owns and removes light so it's ok to be just a pointer
        void registerLight(Light *light);

        // called when actor with light removed
        void unregisterLight(Light *light);

        template <typename T, typename... Args>
            requires std::derived_from<T, Actor>
        std::shared_ptr<T> createActor(Args &&...args)
        {
            auto actor = std::make_shared<T>(
                renderer,
                std::forward<Args>(args)...);
            actor->eventCreated();
            addActor(actor);
            return actor;
        }

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

        inline Renderer *getRenderer()
        {
            return renderer;
        }

        inline std::shared_ptr<Texture> getAtmosphereRadienceTexture()
        {
            return atmosphereRadiance;
        }

    protected:
        void addActor(std::shared_ptr<Actor> actor);

        Renderer *renderer = nullptr;

        std::vector<std::shared_ptr<Actor>> actors;
        std::vector<Light *> lights;
        std::shared_ptr<ActorCamera> actorCamera;
        std::vector<Actor *> blendingPass;

        std::shared_ptr<Texture> atmosphereMap;
        std::shared_ptr<Texture> atmosphereRadiance;
        float atmosphereRadianceFactor = 1.0f;
        std::shared_ptr<Material> atmoMaterial;

        Vector4 ambientLightColor = {0.8f, 0.8f, 0.8f, 1.0f};

        Matrix4x4 mVP = Matrix4x4::identity();
    };
};
