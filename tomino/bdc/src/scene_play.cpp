#include "bdc/src/pch.h"
#include "bdc/include/scene_play.h"

#include "bdc/include/entities.h"

#pragma comment(lib, "riaecs.lib")
#pragma comment(lib, "mono_identity.lib")
#pragma comment(lib, "mono_transform.lib")
#pragma comment(lib, "mono_d3d12.lib")
#pragma comment(lib, "mono_scene.lib")
#pragma comment(lib, "mono_render.lib")
#pragma comment(lib, "mono_physics.lib")
#pragma comment(lib, "mono_object_controller.lib")
#pragma comment(lib, "bdc_player.lib")
#pragma comment(lib, "bdc_dot.lib")
#pragma comment(lib, "bdc_enemy.lib")
#pragma comment(lib, "bdc_game_flow.lib")
#pragma comment(lib, "bdc_cage.lib")

using namespace DirectX;

bdc::PlaySceneEntitiesFactory::PlaySceneEntitiesFactory()
{
}

bdc::PlaySceneEntitiesFactory::~PlaySceneEntitiesFactory()
{
}

riaecs::StagingEntityArea bdc::PlaySceneEntitiesFactory::CreateEntities(
    riaecs::Entity sceneEntity, riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont) const
{
    // Create staging area
    riaecs::StagingEntityArea stagingArea = ecsWorld.CreateStagingArea();

    // Create json file loader
    std::unique_ptr<riaecs::IFileLoader> jsonLoader = std::make_unique<mono_file::FileLoaderJson>();

    // Load config json file
    std::unique_ptr<riaecs::IFileData> configJsonFileData = jsonLoader->Load(bdc::CONFIG_JSON_FILE_PATH);
    assert(configJsonFileData != nullptr);

    // Cast to json data
    mono_file::FileDataJson* configJsonFileDataPtr = dynamic_cast<mono_file::FileDataJson*>(configJsonFileData.get());
    assert(configJsonFileDataPtr != nullptr);

    // Get small config json object 
    const nlohmann::json& configJson = configJsonFileDataPtr->GetJsonData();

    riaecs::Entity playerEntity = bdc::play_scene::CreatePlayer(ecsWorld, assetCont, sceneEntity, stagingArea, configJson);
    riaecs::Entity mainCameraEntity = bdc::play_scene::CreateMainCamera(ecsWorld, assetCont, sceneEntity, stagingArea, playerEntity, configJson);
    bdc::play_scene::CreateLights(ecsWorld, assetCont, sceneEntity, stagingArea, configJson);
    bdc::play_scene::CreateMap(ecsWorld, assetCont, sceneEntity, stagingArea, configJson);
    bdc::play_scene::CreateDots(ecsWorld, assetCont, sceneEntity, stagingArea, configJson);
    bdc::play_scene::CreateLargeEnemies(ecsWorld, assetCont, sceneEntity, stagingArea, configJson);
    bdc::play_scene::CreateSmallEnemies(ecsWorld, assetCont, sceneEntity, stagingArea, configJson);
    riaecs::Entity cageEntity = bdc::play_scene::CreateCage(ecsWorld, assetCont, sceneEntity, stagingArea, configJson);

    std::vector<riaecs::Entity> crystalEntities 
        = bdc::play_scene::CreateCrystals(ecsWorld, assetCont, sceneEntity, stagingArea, configJson);

    riaecs::Entity loadingBackgroundEntity 
        = bdc::menu_scene::CreateLoadingBackground(ecsWorld, assetCont, sceneEntity, stagingArea, configJson);

    std::vector<riaecs::Entity> gameOverUIEntities 
        = bdc::play_scene::CreateGameOverUI(ecsWorld, assetCont, sceneEntity, stagingArea, configJson, loadingBackgroundEntity);

    std::vector<riaecs::Entity> playingUIEntities 
        = bdc::play_scene::CreatePlayingUI(ecsWorld, assetCont, sceneEntity, stagingArea, configJson);

    bdc::play_scene::CreateGameFlow(
        ecsWorld, assetCont, sceneEntity, stagingArea, configJson, 
        playerEntity, cageEntity, gameOverUIEntities, crystalEntities);
    
    return stagingArea;
}

void bdc::PlaySceneEntitiesFactory::PostCreateEntities(
    riaecs::Entity sceneEntity, riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont) const
{
    // Get main window entity
    riaecs::Entity mainWindowEntity = ecsWorld.GetRegisteredEntity((size_t)bdc::EntityRegisterIndex::MAIN_WINDOW);
    assert(mainWindowEntity.IsValid() && "Main Window entity is invalid!");

    // Get window component
    mono_d3d12::ComponentWindowD3D12* window = riaecs::GetComponentWithCheck<mono_d3d12::ComponentWindowD3D12>(
        ecsWorld, mainWindowEntity, mono_d3d12::ComponentWindowD3D12ID(), "Main Window", RIAECS_LOG_LOC);

    // Set scene entity to window component
    window->SetSceneEntity(sceneEntity);

    // Get menu scene entity
    riaecs::Entity menuSceneEntity = ecsWorld.GetRegisteredEntity((size_t)bdc::EntityRegisterIndex::MENU_SCENE);
    assert(menuSceneEntity.IsValid() && "Menu Scene entity is invalid!");

    // Get scene component
    mono_scene::ComponentScene* menuScene = riaecs::GetComponentWithCheck<mono_scene::ComponentScene>(
        ecsWorld, menuSceneEntity, mono_scene::ComponentSceneID(), "Menu Scene", RIAECS_LOG_LOC);

    // Request release menu scene
    menuScene->NeedsReleaseRW()() = true;
}

bdc::PlaySceneSystemListEditCmd::PlaySceneSystemListEditCmd()
{
}

bdc::PlaySceneSystemListEditCmd::~PlaySceneSystemListEditCmd()
{
}

void bdc::PlaySceneSystemListEditCmd::Execute(
    riaecs::ISystemList &systemList, riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont) const
{
    systemList.DestroySystem(bdc_cage::SystemCageID());

    systemList.CreateSystem(mono_d3d12::SystemWindowD3D12ID());
    systemList.CreateSystem(mono_scene::SystemSceneID());
    systemList.CreateSystem(mono_transform::SystemTransformID());
    systemList.CreateSystem(mono_render::SystemRenderID());
    systemList.CreateSystem(mono_physics::SystemPhysicsID());
    systemList.CreateSystem(mono_object_controller::SystemObjectControllerID());
    systemList.CreateSystem(bdc_player::SystemPlayerID());
    systemList.CreateSystem(bdc_enemy::SystemEnemyID());
    systemList.CreateSystem(bdc_game_flow::SystemGameFlowID());
    systemList.CreateSystem(bdc_cage::SystemCageID());

	riaecs::Log::OutToConsole("Play Scene: System List Edited.\n");

    systemList.SetOrder
    ({
        mono_d3d12::SystemWindowD3D12ID(),
        mono_scene::SystemSceneID(),
        bdc_player::SystemPlayerID(),
        mono_object_controller::SystemObjectControllerID(),
        bdc_enemy::SystemEnemyID(),
        bdc_cage::SystemCageID(),
        bdc_game_flow::SystemGameFlowID(),
        // mono_transform::SystemTransformID(),
        mono_physics::SystemPhysicsID(),
        mono_transform::SystemTransformID(),
        mono_render::SystemRenderID()
    });
}

std::unique_ptr<riaecs::ISystemLoopCommand> bdc::PlaySceneSystemListEditCmd::Clone() const
{
    return std::make_unique<PlaySceneSystemListEditCmd>(*this);
}

void bdc::PlaySceneWhenDeadSystemLoopCommand::Execute(
    riaecs::ISystemList &systemList, riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont) const
{
    systemList.CreateSystem(mono_d3d12::SystemWindowD3D12ID());
    systemList.CreateSystem(mono_scene::SystemSceneID());
    systemList.CreateSystem(mono_transform::SystemTransformID());
    systemList.CreateSystem(mono_render::SystemRenderID());

    systemList.SetOrder
    ({
        mono_d3d12::SystemWindowD3D12ID(),
        mono_scene::SystemSceneID(),
        mono_transform::SystemTransformID(),
        mono_render::SystemRenderID()
    });
}

std::unique_ptr<riaecs::ISystemLoopCommand> bdc::PlaySceneWhenDeadSystemLoopCommand::Clone() const
{
    return std::make_unique<PlaySceneWhenDeadSystemLoopCommand>(*this);
}
    
