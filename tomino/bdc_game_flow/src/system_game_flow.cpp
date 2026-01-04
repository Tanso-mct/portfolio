#include "bdc_game_flow/src/pch.h"
#include "bdc_game_flow/include/system_game_flow.h"

#pragma comment(lib, "riaecs.lib")

#include "bdc_player/bdc_player.h"
#pragma comment(lib, "bdc_player.lib")

#include "mono_scene/mono_scene.h"
#pragma comment(lib, "mono_scene.lib")

#include "mono_identity/mono_identity.h"
#pragma comment(lib, "mono_identity.lib")

#include "bdc_game_flow/include/component_game_flow.h"

bdc_game_flow::SystemGameFlow::SystemGameFlow()
{
}

bdc_game_flow::SystemGameFlow::~SystemGameFlow()
{
}

bool bdc_game_flow::SystemGameFlow::Update
(
    riaecs::IECSWorld &ecsWorld, 
    riaecs::IAssetContainer &assetCont, 
    riaecs::ISystemLoopCommandQueue &systemLoopCmdQueue
){
    // Define a structure to hold the game flow related components
    struct GameFlowRelated
    {
        riaecs::Entity gameFlowEntity = riaecs::Entity();
        bdc_game_flow::ComponentGameFlow* gameFlow = nullptr;

        riaecs::Entity playerEntity = riaecs::Entity();
        bdc_player::ComponentPlayer* player = nullptr;
    };

    // Map to hold Entity to GameFlowRelated
    std::unordered_map<riaecs::Entity, GameFlowRelated> gameFlowRelated;

    // Iterate through all entities with ComponentGameFlow
    for (const riaecs::Entity &entity : ecsWorld.View(bdc_game_flow::ComponentGameFlowID())())
    {
        // Get identity component
        mono_identity::ComponentIdentity* identity
        = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
            ecsWorld, entity, mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

        // Skip if identity component is not active
        if (!identity->IsActiveSelf())
            continue;

        // Get game flow component
        bdc_game_flow::ComponentGameFlow* gameFlow
        = riaecs::GetComponentWithCheck<bdc_game_flow::ComponentGameFlow>(
            ecsWorld, entity, bdc_game_flow::ComponentGameFlowID(), "GameFlow", RIAECS_LOG_LOC);

        // Get scene tag component
        mono_scene::ComponentSceneTag* sceneTag
        = riaecs::GetComponentWithCheck<mono_scene::ComponentSceneTag>(
            ecsWorld, entity, mono_scene::ComponentSceneTagID(), "SceneTag", RIAECS_LOG_LOC);

        // Store the pair in the map
        gameFlowRelated[sceneTag->GetSceneEntity()].gameFlowEntity = entity;
        gameFlowRelated[sceneTag->GetSceneEntity()].gameFlow = gameFlow;
    }

    if (gameFlowRelated.empty())
    {
        // No game flow components found, nothing to process
        return true; // Continue running
    }

    // Iterate through all entities with ComponentPlayer
    for (const riaecs::Entity &entity : ecsWorld.View(bdc_player::ComponentPlayerID())())
    {
        // Get identity component
        mono_identity::ComponentIdentity* identity
        = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
            ecsWorld, entity, mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

        // Skip if identity component is not active
        if (!identity->IsActiveSelf())
            continue;

        // Get player component
        bdc_player::ComponentPlayer* player
        = riaecs::GetComponentWithCheck<bdc_player::ComponentPlayer>(
            ecsWorld, entity, bdc_player::ComponentPlayerID(), "Player", RIAECS_LOG_LOC);

        // Get scene tag component
        mono_scene::ComponentSceneTag* sceneTag
        = riaecs::GetComponentWithCheck<mono_scene::ComponentSceneTag>(
            ecsWorld, entity, mono_scene::ComponentSceneTagID(), "SceneTag", RIAECS_LOG_LOC);

        // Map the scene entity to the player component
        gameFlowRelated[sceneTag->GetSceneEntity()].playerEntity = entity;
        gameFlowRelated[sceneTag->GetSceneEntity()].player = player;
    }

    // Process each game flow related set
    for (auto &[pairEntity, related] : gameFlowRelated)
    {
        if (!related.gameFlow || !related.player)
            continue; // Skip if either component is missing

        // Get the state machine
        bdc_game_flow::GameStateMachine &stateMachine = related.gameFlow->GetStateMachine();

        if (related.player->IsGotDot())
            stateMachine.HandleEvent(
                (mono_cycle::Event)bdc_game_flow::GameFlowEvent::DOT_GET, ecsWorld);
        if (related.player->GetDotCollectCount() >= related.gameFlow->GetNeedDotCountToSpawnCrystal())
            stateMachine.HandleEvent(
                (mono_cycle::Event)bdc_game_flow::GameFlowEvent::DOT_COLLECT_COUNT_REACHED, ecsWorld);

        if (related.player->IsGotCrystal())
            stateMachine.HandleEvent(
                (mono_cycle::Event)bdc_game_flow::GameFlowEvent::CRYSTAL_GET, ecsWorld);
        if (related.player->GetCrystalCollectCount() >= related.gameFlow->GetNeedCrystalCountToClearPhase())
            stateMachine.HandleEvent(
                (mono_cycle::Event)bdc_game_flow::GameFlowEvent::CRYSTAL_COLLECT_COUNT_REACHED, ecsWorld);

        if (related.player->IsDead())
            stateMachine.HandleEvent(
                (mono_cycle::Event)bdc_game_flow::GameFlowEvent::PLAYER_DEAD, ecsWorld);
        /*if (related.player->GetDeadCount() >= related.gameFlow->GetNeedDeadCountToGameOver())
            stateMachine.HandleEvent(
                (mono_cycle::Event)bdc_game_flow::GameFlowEvent::PLAYER_DEAD_COUNT_REACHED, ecsWorld);*/
    }

    // Iterate through all entities with ComponentGameFlow
    for (const riaecs::Entity &entity : ecsWorld.View(bdc_game_flow::ComponentGameFlowID())())
    {
        // Get identity component
        mono_identity::ComponentIdentity* identity
        = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
            ecsWorld, entity, mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

        // Skip if identity component is not active
        if (!identity->IsActiveSelf())
            continue;

        // Get game flow component
        bdc_game_flow::ComponentGameFlow* gameFlow
        = riaecs::GetComponentWithCheck<bdc_game_flow::ComponentGameFlow>(
            ecsWorld, entity, bdc_game_flow::ComponentGameFlowID(), "GameFlow", RIAECS_LOG_LOC);

        // Check if there is a current system loop command to enqueue
        if (gameFlow->GetCurrentSystemLoopCommand() != nullptr)
        {
            // Enqueue the current system loop command
            systemLoopCmdQueue.Enqueue(
                std::move(gameFlow->GetCurrentSystemLoopCommand()->Clone()));
        }
    }


    return true; // Continue running
}

BDC_GAME_FLOW_API riaecs::SystemFactoryRegistrar
<bdc_game_flow::SystemGameFlow> bdc_game_flow::SystemGameFlowID;
