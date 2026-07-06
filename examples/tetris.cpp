#include "engine.h"
#include <memory>
#include <vector>
#include <random>
#include <iterator>

#define FIELD_WIDTH 10
#define FIELD_HEIGHT 20

std::shared_ptr<wne::UINodeButton> createButton(
    const char *label,
    std::shared_ptr<wne::Font> font,
    const std::function<void(wne::UINodeButton *)> &action)
{
    const wne::UINodeContainer::Decoration decorationNormal{
        .useBackgroundColor = true,
        .backgroundColor = 0xff777777};
    const wne::UINodeContainer::Decoration decorationHover{
        .useBackgroundColor = true,
        .backgroundColor = 0xffbbbbbb};

    const uint buttonWidth = 400;
    const uint buttonHeight = 80;

    // clang-format off
    return wne::UINodeButton::create(
        wne::UINodeContainer::create(
            wne::UINodeCenter::create(
                wne::UINodeText::create(font, label, 70, 0xffbbbbbb),
                0, buttonHeight - 16
            ),
            buttonWidth, buttonHeight,
            decorationNormal
        ),
        wne::UINodeContainer::create(
            wne::UINodeCenter::create(
                wne::UINodeText::create(font, label, 70, 0xffffffff),
                0, buttonHeight - 16
            ),
            buttonWidth, buttonHeight,
            decorationHover
        ),
        action);
    // clang-format on
}

class Field
{
public:
    Field(std::shared_ptr<wne::Scene> scene)
    {
        this->scene = scene;
        wne::Renderer *renderer = scene->getRenderer();
        auto boxModel = wne::Primitives::createBox(1.0f);
        boxMesh = renderer->createMesh(boxModel);

        addMaterialColor(0xffc25e84);
        addMaterialColor(0xffb15dd6);
        addMaterialColor(0xff916fff);
        addMaterialColor(0xff7196ff);
        addMaterialColor(0xff5fc7ff);
        addMaterialColor(0xff71f8f9);
    }

    void update(float delta, bool isGameStatic)
    {
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

    void genNextFigure()
    {
    }

    void spawnNextFigure()
    {
        genNextFigure();
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
    scene->setAmbientLight(0.6f, 0.6f, 0.6f);

    // images & textures
    auto imageAtmosphere = wne::ImageAtmo::create("./atmosphere.jpg");
    scene->setAtmosphere(
        renderer->createTexture(imageAtmosphere),
        renderer->createTexture(imageAtmosphere->getAtmosphereAsImage()));

    // light
    auto sun = renderer->createLightDirectional();
    sun->setDirection({-0.5, -0.5, -0.5});
    sun->setColor({0.4f, 0.4f, 0.4f});
    auto actorSun = scene->createActor<wne::ActorLight>(sun);

    // camera
    auto camera = wne::CameraPerspective::create(window);
    auto actorCamera = scene->createActor<wne::ActorCamera>(camera);
    actorCamera->setPosition(wne::Vector3(0, 10.0f, -11.2f));
    scene->setCamera(actorCamera);

    // game
    std::unique_ptr<Field> field = std::make_unique<Field>(scene);
    field->setupIntro();

    enum class GameState
    {
        Menu,
        Playing,
        GameOver
    };
    GameState gameState = GameState::Menu;

    // ui
    auto ui = renderer->createScene();

    auto cameraUI = wne::CameraOrtho::create(window);
    auto actorUICamera = ui->createActor<wne::ActorCamera>(cameraUI);
    ui->setCamera(actorUICamera);

    auto uiMainMenu = ui->createActor<wne::ActorUI>(window, 2560, 1440);
    auto uiMainMenuRoot = &uiMainMenu->getRoot();

    const uint padding = 32;

    // clang-format off
    uiMainMenuRoot->setChild(
        wne::UINodeCenter::create(
            wne::UINodeColumn::create(
                {
                    createButton("New Game", nullptr, 
                        [&](wne::UINodeButton *button){
                            field->clear();
                            field->genNextFigure();
                            field->spawnNextFigure();
                            uiMainMenu->setVisibility(false);
                        }
                    ),
                    wne::UINodeContainer::create(padding, padding),
                    createButton("Quit", nullptr, 
                        [&](wne::UINodeButton *button){
                            window->close();
                        }
                    ),
                }
            )
        )
    );

    while (!window->isCloseRequested())
    {
        float delta = wne::Engine::getInstance()->update();
        field->update(delta, gameState!= GameState::Playing);
    }

    return 0;
}