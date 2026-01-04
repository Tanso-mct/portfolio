#include "bdc/src/pch.h"
#include "bdc/include/scene_menu.h"

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

bdc::MenuSceneEntitiesFactory::MenuSceneEntitiesFactory()
{
}

bdc::MenuSceneEntitiesFactory::~MenuSceneEntitiesFactory()
{
}

riaecs::StagingEntityArea bdc::MenuSceneEntitiesFactory::CreateEntities(
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

    // Create entities
    riaecs::Entity mainCameraEntity = bdc::menu_scene::CreateMainCamera(ecsWorld, assetCont, sceneEntity, stagingArea, configJson);
    riaecs::Entity menuBackgroundEntity = bdc::menu_scene::CreateMenuTitle(ecsWorld, assetCont, sceneEntity, stagingArea, configJson);
    riaecs::Entity loadingBackgroundEntity = bdc::menu_scene::CreateLoadingBackground(ecsWorld, assetCont, sceneEntity, stagingArea, configJson);
    riaecs::Entity startButtonEntity = bdc::menu_scene::CreateStartButton(ecsWorld, assetCont, sceneEntity, stagingArea, configJson, loadingBackgroundEntity);
    
    return stagingArea;
}

void bdc::MenuSceneEntitiesFactory::PostCreateEntities(
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

    // Get play scene entity
    riaecs::Entity playSceneEntity = ecsWorld.GetRegisteredEntity((size_t)bdc::EntityRegisterIndex::PLAY_SCENE);
    assert(playSceneEntity.IsValid() && "Play Scene entity is invalid!");

    // Get scene component
    mono_scene::ComponentScene* playScene = riaecs::GetComponentWithCheck<mono_scene::ComponentScene>(
        ecsWorld, playSceneEntity, mono_scene::ComponentSceneID(), "Play Scene", RIAECS_LOG_LOC);

    // Request release play scene
    playScene->NeedsReleaseRW()() = true;
}

bdc::MenuSceneSystemListEditCmd::MenuSceneSystemListEditCmd()
{
}

bdc::MenuSceneSystemListEditCmd::~MenuSceneSystemListEditCmd()
{
}

void bdc::MenuSceneSystemListEditCmd::Execute(
    riaecs::ISystemList &systemList, riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont) const
{
    systemList.CreateSystem(mono_d3d12::SystemWindowD3D12ID());
    systemList.CreateSystem(mono_scene::SystemSceneID());
    systemList.CreateSystem(mono_transform::SystemTransformID());
    systemList.CreateSystem(mono_render::SystemRenderID());
    systemList.CreateSystem(bdc_game_flow::SystemGameFlowID());

    riaecs::Log::OutToConsole("Menu Scene: System List Edited.\n");

    systemList.SetOrder
    ({
        mono_d3d12::SystemWindowD3D12ID(),
        mono_scene::SystemSceneID(),
        bdc_game_flow::SystemGameFlowID(),
        mono_transform::SystemTransformID(),
        mono_render::SystemRenderID()
    });
}

std::unique_ptr<riaecs::ISystemLoopCommand> bdc::MenuSceneSystemListEditCmd::Clone() const
{
    return std::make_unique<MenuSceneSystemListEditCmd>(*this);
}
