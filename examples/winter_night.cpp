#include "engine.h"

void spawnTree(
    float x, float z,
    wne::ActorTerrain *terrain,
    wne::Scene *scene,
    std::shared_ptr<wne::Mesh> mesh,
    std::shared_ptr<wne::Material> material)
{
    auto actorTree = scene->createActor<wne::ActorMesh>(mesh);
    actorTree->setPosition(x, terrain->getHeightGlobal(x, z), z);
    actorTree->setRotation(0.0f, fRand(PI2), 0.0f);
    actorTree->setScale(12.0f + fRand(12.0f));
    actorTree->setMaterial(material);
}

class ActorSnowflake : public wne::ActorSprite
{
public:
    ActorSnowflake(wne::Renderer *renderer, wne::ActorTerrain *actorTerrain) : wne::ActorSprite(renderer)
    {
        this->actorTerrain = actorTerrain;
        if (!materialSnowFlake)
        {
            auto snowImage = wne::Image::create("./snowflake.png");
            auto snowTexture = renderer->createTexture(snowImage);
            materialSnowFlake = renderer->createFlatMaterial(snowTexture);
            materialSnowFlake->setNormalShadowingFactor(1.0f);
            materialSnowFlake->setMasked(true);
        }
        this->setMaterial(materialSnowFlake);
        setScale(0.12f);
        genNewPosition();
    }

    static std::shared_ptr<ActorSnowflake> create(wne::Renderer *renderer, wne::ActorTerrain *actorTerrain)
    {
        return std::make_shared<ActorSnowflake>(renderer, actorTerrain);
    }

    void eventUpdate(float delta) override final
    {
        position.y -= delta * 12.0f;
        if (position.y < actorTerrain->getHeightGlobal(position.x, position.z))
            genNewPosition();
        moveCycle += delta;

        float movement = sinf(moveCycle) * delta * 10.0f;
        position.x += movement * moveNormalX;
        position.z += movement * moveNormalZ;
    }

    void genNewPosition()
    {
        position = wne::Vector3(fRand(160.0f) - 80.0f, fRand(40.0f) + 40.0f, fRand(160.0f) - 80.0f);
        moveCycle = fRand(PI2);
        float moveRotation = fRand(PI2);
        moveNormalX = sinf(moveRotation);
        moveNormalZ = cosf(moveRotation);
    }

protected:
    static std::shared_ptr<wne::Material> materialSnowFlake;
    wne::ActorTerrain *actorTerrain = nullptr;
    float moveCycle = 0.0f, moveNormalX = 0, moveNormalZ = 0;
};
std::shared_ptr<wne::Material> ActorSnowflake::materialSnowFlake;

int main()
{
    wne::Engine::getInstance()->enableLogger();

    auto window = wne::Window::create("Winter Night", 2560, 1440);
    if (!window)
        return 1;

    window->setMouseCapture(true);
    auto renderer = window->getRenderer();
    renderer->setSyncState(false);
    renderer->setMSAASampleCount(4);

    const std::vector<wne::VertexTextured> vertices = {
        {{-5.0f, -5.0f, 0.0f}, {0.0f, 1.0f}, {0, 0, -1.0f}},
        {{5.0f, -5.0f, 0.0f}, {1.0f, 1.0f}, {0, 0, -1.0f}},
        {{5.0f, 5.0f, 0.0f}, {1.0f, 0.0f}, {0, 0, -1.0f}},
        {{-5.0f, 5.0f, 0.0f}, {0.0f, 0.0f}, {0, 0, -1.0f}}};

    const std::vector<uint16_t> indices = {0, 2, 1, 3, 2, 0};

    // scene
    auto scene = renderer->createScene();
    scene->setAmbientLight(0.0f, 0.0f, 0.0f);

    // images
    auto imageAtmosphere = wne::ImageAtmo::create("./atmosphereNight2.jpg");
    scene->setAtmosphere(
        renderer->createTexture(imageAtmosphere),
        renderer->createTexture(imageAtmosphere->getAtmosphereAsImage()),
        0.8f);

    auto snowImageHeight = wne::Image::create("./snow_height.png");
    auto snowImage = wne::Image::create("./snow_defuse.png");
    auto snowTexture = renderer->createTexture(snowImage);
    auto snowMaterial = renderer->createFlatMaterial(snowTexture);

    auto treeBark = wne::Image::create("./pine_bark.jpg");
    auto textureTreeBark = renderer->createTexture(treeBark);
    auto materialTreeBark = renderer->createFlatMaterial(textureTreeBark);

    auto actorTerrain = scene->createActor<wne::ActorTerrain>(60, 160.0f, 16.0f, snowImageHeight);
    actorTerrain->setMaterial(snowMaterial);
    actorTerrain->setPosition(0, 0, 0);

    auto treeModel = wne::Loader3d::loadAsModel("./tree1.fbx");
    auto treeMesh = renderer->createMesh(treeModel);
    spawnTree(-75.0f, 20, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(49.0f, 21, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-28.0f, 17, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(59.0f, 19, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-40.0f, 14, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(39.0f, 16, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-40.0f, 11.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(44.0f, 10.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-50.0f, 5, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(55.0f, 6, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-40.0f, 11.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(44.0f, 10.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-70.0f, 0, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(70.0f, 0, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-60.0f, -11.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(65.0f, -10.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-52.0f, -14.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(56.0f, -8.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-44.0f, -13.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(32.0f, -15.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-42.0f, -16.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(30.0f, -17.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-42.0f, 6.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(30.0f, 6.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-46.0f, 9.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(36.0f, 9.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-70.0f, -20.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(75.0f, -21.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-42.0f, -22.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(62.0f, -24.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-48.0f, -25.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(38.0f, -27.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-46.0f, -25.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(62.0f, -26.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-44.0f, -27.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(68.0f, -28.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-77.0f, -31.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(45.0f, -32.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-41.0f, -33.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(35.0f, -34.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-65.0f, -35.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(32.0f, -36.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-53.0f, -37.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(46.0f, -38.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-44.0f, -39.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(31.0f, -40.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-43.0f, -41.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(49.0f, -42.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-75.0f, -43.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(58.0f, -44.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-63.0f, -45.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(54.0f, -46.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-42.0f, -47.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(32.0f, -48.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-63.0f, -49.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(53.0f, -50.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-43.0f, -51.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(41.0f, -52.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-47.0f, -53.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(65.0f, -54.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-70.0f, -55.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(78.0f, -56.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-79.0f, -57.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(54.0f, -58.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-23.0f, -65.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(47.0f, -66.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-77.0f, -67.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(64.0f, -68.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-45.0f, -72.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(32.0f, -73.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-33.0f, -74.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(63.0f, -75.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-46.0f, -76.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(33.0f, -77.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(44.0f, -78.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-36.0f, -71.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(21.0f, -67.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(36.0f, -72.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);
    spawnTree(-49.0f, -69.0f, actorTerrain.get(), scene.get(), treeMesh, materialTreeBark);

    const uint AmountOfSnowflakes = 1600;
    for (uint i = 0; i < AmountOfSnowflakes; i++)
        scene->createActor<ActorSnowflake>(actorTerrain.get());

    // light
    auto sun = renderer->createLightDirectional();
    sun->setDirection({-0.5, 0.9, 0.5});
    sun->setColor({0.02f, 0.02f, 0.04f});
    auto actorSun = scene->createActor<wne::ActorLight>(sun);
    sun->enableShadows(1, 2048);

    auto spot = renderer->createLightSpot();
    spot->setDirection({0, -1.0f, 0});
    spot->setColor({1.2f, 1.2f, 1.1f});
    spot->setAffectRadius(30.0f);
    spot->setOuterCutOff(0.3f);
    auto actorSpot = scene->createActor<wne::ActorLight>(spot);
    actorSpot->setPosition(0, 28.0f, 20.0f);

    // camera
    auto camera = wne::CameraPerspective::create(window);
    camera->setFarPlane(160.0f);
    auto actorCamera = scene->createActor<wne::ActorCameraFlying>(camera, window);
    actorCamera->setPosition(0, 16.0f, 0);
    scene->setCamera(actorCamera);

    float translate = 0.0f;
    while (!window->isCloseRequested())
    {
        float delta = wne::Engine::getInstance()->update();
        translate += delta * 2.0f;
    }

    return 0;
}
