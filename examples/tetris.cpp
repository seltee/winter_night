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
    struct FigureBlock
    {
        int x, y;
        std::shared_ptr<wne::ActorMesh> block;
    };

    Field(std::shared_ptr<wne::Scene> scene, std::shared_ptr<wne::Window> eventWindow)
    {
        eventsSubscription = eventWindow->subscribe();

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
        wne::WindowEvents::WindowEvent event;
        bool buttonRotatePressed = false;
        while (eventsSubscription->getEvent(&event))
        {
            if (event.type == wne::WindowEvents::WindowEventType::KEY_PRESS)
            {
                if (event.key.code == wne::Key::Arrowleft)
                    buttonMoveLeftPressed = true;
                if (event.key.code == wne::Key::ArrowRight)
                    buttonMoveRightPressed = true;
                if (event.key.code == wne::Key::ArrowDown)
                    buttonMoveDownPressed = true;
                if (event.key.code == wne::Key::Spacebar)
                    buttonRotatePressed = true;
            }
            else if (event.type == wne::WindowEvents::WindowEventType::KEY_RELEASE)
            {
                if (event.key.code == wne::Key::Arrowleft)
                {
                    buttonMoveLeftPressed = false;
                    timerMoveLeft = 0.0f;
                }
                if (event.key.code == wne::Key::ArrowRight)
                {
                    buttonMoveRightPressed = false;
                    timerMoveRight = 0.0f;
                }
                if (event.key.code == wne::Key::ArrowDown)
                    buttonMoveDownPressed = false;
            }
        }

        if (isGameStatic)
            return;

        timerMoveDown += delta;
        if (timerMoveDown >= 0.64f)
        {
            timerMoveDown = 0.0f;
            activeFigureMoveDown();
        }
        timerMoveLeft -= delta;
        if (timerMoveLeft < 0.0f)
            timerMoveLeft = 0.0f;
        timerMoveRight -= delta;
        if (timerMoveRight < 0.0f)
            timerMoveRight = 0.0f;

        if (buttonMoveLeftPressed && timerMoveLeft <= 0)
        {
            timerMoveLeft = 0.1f;
            activeFigureMoveLeft();
        }
        if (buttonMoveRightPressed && timerMoveRight <= 0)
        {
            timerMoveRight = 0.1f;
            activeFigureMoveRight();
        }
        if (buttonMoveDownPressed && timerMoveDown < 0.6f)
        {
            timerMoveDown = 0.6f;
        }
        if (buttonRotatePressed)
        {
            rotateActiveFigure();
        }
    }

    void setupIntro()
    {
        clear();
        for (uint y = 0; y < FIELD_HEIGHT; y++)
        {
            for (uint x = 0; x < FIELD_WIDTH; x++)
            {
                addBlock(x, y, getRandomMaterial());
            }
        }
    }

    void clear()
    {
        if (blocks.size() > 0)
        {
            for (auto &block : blocks)
            {
                block.block->destroy();
            }
            blocks.clear();
        }
    }

    void genNextFigure()
    {
        removeNextFigure();
        int figure = getRandomNumber(0, 4);
        nextFigureMaterial = getRandomMaterial();
        // todo move to insert
        if (figure == 0)
        {
            nextFigure.push_back({0, 0});
            nextFigure.push_back({1, 0});
            nextFigure.push_back({0, 1});
            nextFigure.push_back({1, 1});
        }
        else if (figure == 1)
        {
            nextFigure.push_back({0, 0});
            nextFigure.push_back({1, 0});
            nextFigure.push_back({1, 1});
            nextFigure.push_back({2, 1});
        }
        else if (figure == 2)
        {
            nextFigure.push_back({0, 1});
            nextFigure.push_back({1, 1});
            nextFigure.push_back({1, 0});
            nextFigure.push_back({2, 0});
        }
        else if (figure == 3)
        {
            nextFigure.push_back({0, 0});
            nextFigure.push_back({0, 1});
            nextFigure.push_back({0, 2});
            nextFigure.push_back({0, 3});
        }
        else if (figure == 4)
        {
            nextFigure.push_back({0, 0});
            nextFigure.push_back({0, 1});
            nextFigure.push_back({0, 2});
            nextFigure.push_back({1, 2});
        }
        else if (figure == 5)
        {
            nextFigure.push_back({1, 0});
            nextFigure.push_back({1, 1});
            nextFigure.push_back({1, 2});
            nextFigure.push_back({0, 2});
        }
        else if (figure == 6)
        {
            nextFigure.push_back({0, 0});
            nextFigure.push_back({1, 0});
            nextFigure.push_back({2, 0});
            nextFigure.push_back({1, 1});
        }

        int rotate = getRandomNumber(0, 4);
        for (int i = 0; i < rotate; i++)
        {
            // rotateNextFigure();
        }

        for (auto &nextFigureElement : nextFigure)
        {
            nextFigureElement.block = createBox(nextFigureMaterial);
            setToGrid(nextFigureElement.block, 12 + nextFigureElement.x, 12 - nextFigureElement.y);
        }
    }

    std::shared_ptr<wne::ActorMesh> createBox(std::shared_ptr<wne::Material> material)
    {
        auto actorBox = scene->createActor<wne::ActorMesh>(boxMesh);
        actorBox->setMaterial(material);
        return actorBox;
    }

    void spawnNextFigure()
    {
        removeActiveFigure();

        for (auto &nextFigureElement : nextFigure)
        {
            int x = 4 + nextFigureElement.x;
            int y = 21 - nextFigureElement.y;

            auto actorBox = scene->createActor<wne::ActorMesh>(boxMesh);
            actorBox->setMaterial(nextFigureMaterial);
            activeFigure.push_back({x, y, actorBox});
            setToGrid(actorBox, x, y);
        }
    }

    void removeActiveFigure()
    {
        for (auto &activeFigureElement : activeFigure)
        {
            activeFigureElement.block->destroy();
        }
        activeFigure.clear();
    }

    uint getFigureX(std::vector<FigureBlock> &figure)
    {
        int minX = 100;
        for (auto &figureElement : figure)
            minX = std::min(figureElement.x, minX);
        return minX;
    }

    uint getFigureY(std::vector<FigureBlock> &figure)
    {
        int minY = 100;
        for (auto &figureElement : figure)
            minY = std::min(figureElement.y, minY);
        return minY;
    }

    uint getFigureWidth(std::vector<FigureBlock> &figure)
    {
        int minX = 100;
        int maxX = 0;
        for (auto &figureElement : figure)
        {
            minX = std::min(figureElement.x, minX);
            maxX = std::max(figureElement.x, maxX);
        }
        return maxX - minX + 1;
    }

    uint getFigureHeight(std::vector<FigureBlock> &figure)
    {
        int minY = 100;
        int maxY = 0;
        for (auto &figureElement : figure)
        {
            minY = std::min(figureElement.y, minY);
            maxY = std::max(figureElement.y, maxY);
        }
        return maxY - minY + 1;
    }

    void removeNextFigure()
    {
        for (auto &nextFigureBlock : nextFigure)
        {
            nextFigureBlock.block->destroy();
        }
        nextFigure.clear();
    }

    std::shared_ptr<wne::ActorMesh> addBlock(int x, int y, std::shared_ptr<wne::Material> material)
    {
        auto actorBox = createBox(material);
        setToGrid(actorBox, x, y);
        blocks.push_back({x, y, actorBox});
        return actorBox;
    }

    void setToGrid(std::shared_ptr<wne::ActorMesh> block, int x, int y)
    {
        block->setPosition((float)(x - FIELD_WIDTH / 2), (float)y, 0);
    }

    void activeFigureMoveDown()
    {
        activeFigureMove(0, -1);
    }

    void activeFigureMoveLeft()
    {
        activeFigureMove(-1, 0);
    }

    void activeFigureMoveRight()
    {
        activeFigureMove(1, 0);
    }

    void activeFigureMove(int x, int y)
    {
        if (canMove(activeFigure, x, y))
        {
            for (auto &activeFigureElement : activeFigure)
            {
                activeFigureElement.x += x;
                activeFigureElement.y += y;
                setToGrid(activeFigureElement.block, activeFigureElement.x, activeFigureElement.y);
            }
        }
        else
        {
            if (y != 0)
            {
                placeFigure();
            }
        }
    }

    bool canMove(std::vector<FigureBlock> &figure, int x, int y)
    {
        for (auto &figureElement : figure)
        {
            int checkX = (int)figureElement.x + x;
            int checkY = (int)figureElement.y + y;
            if (checkY < 0)
                return false;
            if (checkX < 0)
                return false;
            if (checkX >= FIELD_WIDTH)
                return false;

            for (auto &checkFigureElement : blocks)
            {
                if ((int)checkFigureElement.x == checkX && (int)checkFigureElement.y == checkY)
                    return false;
            }
        }
        return true;
    }

    void placeFigure()
    {
        for (auto &activeFigureElement : activeFigure)
        {
            blocks.push_back(activeFigureElement);
        }
        activeFigure.clear();
        spawnNextFigure();
        genNextFigure();
    }

    void rotateActiveFigure()
    {
        rotateFigure(activeFigure, true);
    }

    void rotateNextFigure()
    {
        rotateFigure(nextFigure, false);
    }

    void rotateFigure(std::vector<FigureBlock> &blocks, bool checkCollisions)
    {
        std::vector<FigureBlock> backup = blocks;

        int x = getFigureX(blocks);
        int y = getFigureY(blocks);
        int width = getFigureWidth(blocks);
        int height = getFigureHeight(blocks);
        int rQuad = std::max(width, height);
        int minRelX = 100;
        int minRelY = 100;

        for (auto &figureElement : blocks)
        {
            int relX = figureElement.x - x;
            int relY = figureElement.y - y;
            int newX = rQuad - 1 - relY;
            int newY = relX;
            figureElement.x += newX - relX;
            figureElement.y += newY - relY;
            minRelX = std::min(minRelX, newX);
            minRelY = std::min(minRelY, y - figureElement.y);
        }

        for (auto &figureElement : blocks)
        {
            figureElement.x -= minRelX;
            figureElement.y += (height - 1) + minRelY;
        }

        if (checkCollisions)
        {
            int minX = 0;
            int maxX = FIELD_WIDTH - 1;
            for (auto &figureElement : blocks)
            {
                minX = std::min(minX, figureElement.x);
                maxX = std::max(maxX, figureElement.x);
            }
            maxX -= (FIELD_WIDTH - 1);

            for (auto &figureElement : blocks)
                figureElement.x -= minX + maxX;

            if (!canMove(blocks, 0, 0))
            {
                blocks = backup;
                return;
            }
        }

        for (auto &figureElement : blocks)
            setToGrid(figureElement.block, figureElement.x, figureElement.y);
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
        return materials[getRandomNumber(0, materials.size() - 1)];
    }

    int getRandomNumber(int min, int max)
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(min, max);
        return dis(gen);
    }

protected:
    std::shared_ptr<wne::WindowEvents> eventsSubscription;
    float timerMoveDown = 0.0f;
    float timerMoveLeft = 0.0f;
    float timerMoveRight = 0.0f;

    bool buttonMoveDownPressed = false;
    bool buttonMoveLeftPressed = false;
    bool buttonMoveRightPressed = false;

    std::shared_ptr<wne::Scene> scene;
    std::shared_ptr<wne::Mesh> boxMesh;
    std::vector<FigureBlock> blocks;
    std::vector<std::shared_ptr<wne::Material>> materials;

    std::vector<FigureBlock> nextFigure;
    std::shared_ptr<wne::Material> nextFigureMaterial;

    std::vector<FigureBlock> activeFigure;
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
    std::unique_ptr<Field> field = std::make_unique<Field>(scene, window);
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
                            gameState = GameState::Playing;
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