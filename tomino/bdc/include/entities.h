#pragma once
#include "riaecs/riaecs.h"

#include <vector>

namespace bdc
{

constexpr const char* CONFIG_JSON_FILE_PATH = "../resources/bdc/config.json";
constexpr uint32_t BACK_BUFFER_COUNT = 2;

enum class EntityRegisterIndex : size_t
{
    MAIN_WINDOW,
    MENU_SCENE,
    PLAY_SCENE,
};

// Create menu scene
riaecs::Entity CreateMenuScene(riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont);

// Create play scene
riaecs::Entity CreatePlayScene(riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont);

// Create main window
riaecs::Entity CreateMainWindow(
    riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, const riaecs::Entity &sceneEntity);

namespace menu_scene
{
    riaecs::Entity CreateMainCamera
    (
        riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
        const riaecs::Entity &menuSceneEntity, riaecs::StagingEntityArea &stagingArea,
        const nlohmann::json& configJson
    );

    riaecs::Entity CreateMenuTitle
    (
        riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
        const riaecs::Entity &menuSceneEntity, riaecs::StagingEntityArea &stagingArea,
        const nlohmann::json& configJson
    );

    riaecs::Entity CreateStartButton
    (
        riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
        const riaecs::Entity &menuSceneEntity, riaecs::StagingEntityArea &stagingArea,
        const nlohmann::json& configJson,
        const riaecs::Entity& loadingBackgroundEntity
    );

    riaecs::Entity CreateLoadingBackground
    (
        riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
        const riaecs::Entity &menuSceneEntity, riaecs::StagingEntityArea &stagingArea,
        const nlohmann::json& configJson
    );
}

namespace play_scene
{
    riaecs::Entity CreateMainCamera
    (
        riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
        const riaecs::Entity &playSceneEntity, riaecs::StagingEntityArea &stagingArea,
        const riaecs::Entity &playerEntity, const nlohmann::json& configJson
    );

    void CreateLights
    (
        riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
        const riaecs::Entity &playSceneEntity, riaecs::StagingEntityArea &stagingArea,
        const nlohmann::json& configJson
    );

    riaecs::Entity CreatePlayer
    (
        riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
        const riaecs::Entity &playSceneEntity, riaecs::StagingEntityArea &stagingArea, const nlohmann::json& configJson
    );

    void CreateMapElement(
        riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
        const riaecs::Entity &playSceneEntity, riaecs::StagingEntityArea &stagingArea,
        const std::string &namePrefix, 
        size_t modelAssetSourceID, size_t albedoTexAssetSourceID, 
        size_t locatorAssetSourceID, size_t boundingBoxAssetSourceID, const nlohmann::json& configJson,
        size_t materialHandleID
    );

    void CreateMapElement(
        riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
        const riaecs::Entity &playSceneEntity, riaecs::StagingEntityArea &stagingArea,
        const std::string &namePrefix, 
        size_t modelAssetSourceID, size_t locatorAssetSourceID, size_t boundingBoxAssetSourceID, 
        const nlohmann::json& configJson, size_t materialHandleID
    );

    void CreateMapElement(
        riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
        const riaecs::Entity &playSceneEntity, riaecs::StagingEntityArea &stagingArea,
        const std::string &namePrefix, 
        size_t modelAssetSourceID, size_t albedoTexAssetSourceID, size_t locatorAssetSourceID,
        size_t materialHandleID
    );

    void CreateMap
    (
        riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
        const riaecs::Entity &playSceneEntity, riaecs::StagingEntityArea &stagingArea, const nlohmann::json& configJson
    );

    void CreateDots
    (
        riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
        const riaecs::Entity &playSceneEntity, riaecs::StagingEntityArea &stagingArea, const nlohmann::json& configJson
    );

    void CreateLargeEnemies
    (
        riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
        const riaecs::Entity &playSceneEntity, riaecs::StagingEntityArea &stagingArea, const nlohmann::json& configJson
    );

    void CreateSmallEnemies
    (
        riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
        const riaecs::Entity &playSceneEntity, riaecs::StagingEntityArea &stagingArea, const nlohmann::json& configJson
    );

    void CreateGameFlow
    (
        riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
        const riaecs::Entity &playSceneEntity, riaecs::StagingEntityArea &stagingArea, 
        const nlohmann::json& configJson, 
        const riaecs::Entity &playerEntity, const riaecs::Entity &cageEntity,
        const std::vector<riaecs::Entity> &gameOverUIEntities,
        const std::vector<riaecs::Entity> &crystalEntities
    );

    riaecs::Entity CreateCage
    (
        riaecs::IECSWorld& ecsWorld, riaecs::IAssetContainer& assetCont, 
        const riaecs::Entity& playSceneEntity, riaecs::StagingEntityArea& stagingArea, const nlohmann::json& configJson
    );

    std::vector<riaecs::Entity> CreateCrystals
    (
        riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
        const riaecs::Entity &playSceneEntity, riaecs::StagingEntityArea &stagingArea, const nlohmann::json& configJson
    );    

    std::vector<riaecs::Entity> CreatePlayingUI
    (
        riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
        const riaecs::Entity &playSceneEntity, riaecs::StagingEntityArea &stagingArea, 
        const nlohmann::json& configJson
    );

    std::vector<riaecs::Entity> CreateGameOverUI
    (
        riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
        const riaecs::Entity &playSceneEntity, riaecs::StagingEntityArea &stagingArea, 
        const nlohmann::json& configJson,
        const riaecs::Entity& loadingBackgroundEntity
    );

} // namespace play_scene


} // namespace bdc