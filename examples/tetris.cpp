#include "engine.h"
#include <memory>
#include <vector>
#include <random>
#include <iterator>

#define FIELD_WIDTH 10
#define FIELD_HEIGHT 20

class Field
{
public:
    Field(std::shared_ptr<wne::Scene> scene)
    {
        this->scene = scene;
        wne::Renderer *renderer = scene->getRenderer();
        auto boxModel = wne::Primitives::createBox(1.0f);
        boxMesh = renderer->createMesh(boxModel);

        addMaterialColor(0xffff0000);
        addMaterialColor(0xff00ff00);
        addMaterialColor(0xff0000ff);
    }

    void setupIntro()
    {
        clear();
        for (uint y = 0; y < FIELD_HEIGHT; y++)
        {
            for (uint x = 0; x < FIELD_WIDTH; x++)
            {
                auto block = addBlock();
                setToGrid(block, x, y);
            }
        }
    }

    void clear()
    {
        if (blocks.size() > 0)
        {
            for (auto &block : blocks)
            {
                block->destroy();
            }
            blocks.clear();
        }
    }

    std::shared_ptr<wne::ActorMesh> addBlock()
    {
        auto actorBox = scene->createActor<wne::ActorMesh>(boxMesh);
        actorBox->setMaterial(getRandomMaterial());
        blocks.push_back(actorBox);
        return actorBox;
    }

    void setToGrid(std::shared_ptr<wne::ActorMesh> block, int x, int y)
    {
        block->setPosition((float)(x - FIELD_WIDTH / 2), (float)y, 0);
    }

    void addMaterialColor(uint32 color)
    {
        wne::Renderer *renderer = scene->getRenderer();
        uint32 data[] = {color, color, color, color};
        auto image = wne::Image::createFromMemory((uint8 *)data, 2, 2);
        auto texture = renderer->createTexture(image);
        materials.push_back(renderer->createFlatMaterial(texture));
    }

    std::shared_ptr<wne::Material> getRandomMaterial()
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, std::distance(materials.begin(), materials.end()) - 1);
        auto pos = materials.begin();
        std::advance(pos, dis(gen));
        return *pos;
    }

protected:
    std::shared_ptr<wne::Scene> scene;
    std::shared_ptr<wne::Mesh> boxMesh;
    std::vector<std::shared_ptr<wne::ActorMesh>> blocks;
    std::vector<std::shared_ptr<wne::Material>> materials;
};

int main()
{
    wne::Engine::getInstance()->enableLogger();

    auto window = wne::Window::create("Tetris", 2560, 1440);
    if (!window)
        return 1;

    window->setMouseCapture(true);
    auto renderer = window->getRenderer();
    renderer->setSyncState(false);

    // scene
    auto scene = renderer->createScene();
    scene->setAmbientLight(0.5f, 0.5f, 0.5f);

    // images & textures
    auto imageAtmosphere = wne::ImageAtmo::create("./atmosphere.jpg");
    scene->setAtmosphere(
        renderer->createTexture(imageAtmosphere),
        renderer->createTexture(imageAtmosphere->getAtmosphereAsImage()));

    // light
    auto sun = renderer->createLightDirectional();
    sun->setDirection({-0.5, -0.5, -0.5});
    sun->setColor({4.0f, 4.0f, 4.0f});
    auto actorSun = scene->createActor<wne::ActorLight>(sun);

    // camera
    auto camera = wne::CameraPerspective::create(window);
    auto actorCamera = scene->createActor<wne::ActorCamera>(camera);
    actorCamera->setPosition(wne::Vector3(0, 10.0f, -11.2f));
    scene->setCamera(actorCamera);

    // game
    std::unique_ptr<Field> field = std::make_unique<Field>(scene);
    field->setupIntro();

    while (!window->isCloseRequested())
    {
        float delta = wne::Engine::getInstance()->update();
    }

    return 0;
}