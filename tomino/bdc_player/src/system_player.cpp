#include "bdc_player/src/pch.h"
#include "bdc_player/include/system_player.h"

#pragma comment(lib, "riaecs.lib")
#pragma comment(lib, "mono_identity.lib")
#pragma comment(lib, "mono_transform.lib")
#pragma comment(lib, "mono_object_controller.lib")
#pragma comment(lib, "mono_d3d12.lib")
#pragma comment(lib, "mono_input_monitor.lib")
#pragma comment(lib, "mono_render.lib")
#pragma comment(lib, "mono_physics.lib")
#pragma comment(lib, "mono_delta_time.lib")
#pragma comment(lib, "bdc_dot.lib")
#pragma comment(lib, "bdc_enemy.lib")
#pragma comment(lib, "bdc_cage.lib")
#pragma comment(lib, "bdc_interactive_object.lib")

using namespace DirectX;

#include "mono_render/include/material_handle_manager.h"

#include "bdc_player/include/component_player.h"
#include "bdc_player/include/component_player_normal_state.h"
#include "bdc_player/include/component_player_enhanced_state.h"

bdc_player::SystemPlayer::SystemPlayer()
{
}

bdc_player::SystemPlayer::~SystemPlayer()
{
}

bool bdc_player::SystemPlayer::Update
(
    riaecs::IECSWorld &ecsWorld, 
    riaecs::IAssetContainer &assetCont,
    riaecs::ISystemLoopCommandQueue &systemLoopCmdQueue
){
    // Update delta time
    deltaTimeProvider_.UpdateTime();

    // Iterate through all player components
    std::vector<riaecs::Entity> playersToToggle;
    std::vector<riaecs::Entity> playersToEnhance;
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

        // Get handler window entity from player component
        riaecs::Entity handlerWindowEntity = player->GetHandlerWindowEntity();

         // Get window component
        mono_d3d12::ComponentWindowD3D12* window
        = riaecs::GetComponentWithCheck<mono_d3d12::ComponentWindowD3D12>(
            ecsWorld, handlerWindowEntity, mono_d3d12::ComponentWindowD3D12ID(), "WindowD3D12", RIAECS_LOG_LOC);
            
        // Get input states
        const mono_input_monitor::KeyboardInputState &keyboardState = window->GetKeyboardState();

        // Check for mode toggle key press
        if (mono_input_monitor::GetKeyDown(keyboardState, player->GetModeToggleKey()))
        {
            // Toggle between normal and enhanced state
            if (ecsWorld.HasComponent(entity, bdc_player::ComponentPlayerNormalStateID()))
                playersToEnhance.push_back(entity);
            else if (ecsWorld.HasComponent(entity, bdc_player::ComponentPlayerEnhancedStateID()))
                playersToToggle.push_back(entity);
            else
                assert(false && "Player entity has neither normal nor enhanced state component.");
        }

        // Get collider component
        mono_physics::ComponentBoxCollider* collider
        = riaecs::GetComponentWithCheck<mono_physics::ComponentBoxCollider>(
            ecsWorld, entity, mono_physics::ComponentBoxColliderID(), "BoxCollider", RIAECS_LOG_LOC);

        // Get collision result
        const mono_physics::BoxCollisionResult &collisionResult = collider->GetBoxCollisionResult();

        if (collisionResult.IsCollided())
        {
            // If collided with any entity tagged as "EnhanceZone", enhance the player
            for (const riaecs::Entity &collidedEntity : collisionResult.GetCollidedEntities())
            {
                // Check if collided entity is dot
                if (ecsWorld.HasComponent(collidedEntity, bdc_dot::ComponentDotID()))
                {
                    // Get dot's identity component
                    mono_identity::ComponentIdentity* dotIdentity
                    = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                        ecsWorld, collidedEntity, mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

                    // Set dot to inactive
                    dotIdentity->SetActiveSelf(false);

                    // Set player got dot flag
                    player->SetIsGotDot(true);

                    // Increment dot collect count
                    uint32_t currentDotCount = player->GetDotCollectCount();
                    player->SetDotCollectCount(currentDotCount + 1);
                    player->SetTotalDotCollectCount(player->GetTotalDotCollectCount() + 1);
                }
                // Check if collided entity is enemy
                else if (ecsWorld.HasComponent(collidedEntity, bdc_enemy::ComponentEnemyID()))
                {
                    // Set player dead flag
                    player->SetIsDead(true);

                    // Increment dead count
                    uint32_t currentDeadCount = player->GetDeadCount();
                    player->SetDeadCount(currentDeadCount + 1);
                }
                // Check if collided entity is cage
                else if (ecsWorld.HasComponent(collidedEntity, bdc_cage::ComponentCageChildID()))
                {
                    // Set player dead flag
                    player->SetIsDead(true);

                    // Increment dead count
                    uint32_t currentDeadCount = player->GetDeadCount();
                    player->SetDeadCount(currentDeadCount + 1);
                }
                // Check if collided entity is crystal
                else if (ecsWorld.HasComponent(collidedEntity, bdc_interactive_object::ComponentCrystalID()))
                {
                    // Get crystal's identity component
                    mono_identity::ComponentIdentity* crystalIdentity
                    = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                        ecsWorld, collidedEntity, mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

                    // Set crystal to inactive
                    crystalIdentity->SetActiveSelf(false);

                    // Get crystal component
                    bdc_interactive_object::ComponentCrystal* crystal
                    = riaecs::GetComponentWithCheck<bdc_interactive_object::ComponentCrystal>(
                        ecsWorld, collidedEntity, bdc_interactive_object::ComponentCrystalID(), "Crystal", RIAECS_LOG_LOC);

                    // Mark crystal as taken
                    crystal->SetIsTaken(true);

                    // Set player got crystal flag
                    player->SetIsGotCrystal(true);

                    // Increment crystal collect count
                    uint32_t currentCrystalCount = player->GetCrystalCollectCount();
                    player->SetCrystalCollectCount(currentCrystalCount + 1);
                    player->SetTotalCrystalCollectCount(player->GetTotalCrystalCollectCount() + 1);
                }
                // // Check if collided entity is item
                // else if (ecsWorld.HasComponent(collidedEntity, bdc_item::ComponentItemID()))
                // {
                //     // Get item's identity component
                //     mono_identity::ComponentIdentity* itemIdentity
                //     = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                //         ecsWorld, collidedEntity, mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

                //     // Set item to inactive
                //     itemIdentity->SetActiveSelf(false);

                //     // Set player got item flag
                //     player->SetIsGotItem(true);
                // }
            }
        }
    }

    // Process players to toggle to normal state
    for (const riaecs::Entity &entity : playersToToggle)
    {
        ecsWorld.RemoveComponent(entity, bdc_player::ComponentPlayerEnhancedStateID());
        ecsWorld.AddComponent(entity, bdc_player::ComponentPlayerNormalStateID());
    }

    // Process players to enhance state
    for (const riaecs::Entity &entity : playersToEnhance)
    {
        ecsWorld.RemoveComponent(entity, bdc_player::ComponentPlayerNormalStateID());
        ecsWorld.AddComponent(entity, bdc_player::ComponentPlayerEnhancedStateID());
    }

    // Iterate through all player normal state components
    for (const riaecs::Entity &entity : ecsWorld.View(bdc_player::ComponentPlayerNormalStateID())())
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

        // Get object controller component
        mono_object_controller::ComponentObjectController* controller
        = riaecs::GetComponentWithCheck<mono_object_controller::ComponentObjectController>(
            ecsWorld, entity, mono_object_controller::ComponentObjectControllerID(), "ObjectController", RIAECS_LOG_LOC);

        // Set normal move speed
        controller->SetMoveSpeed(player->GetMoveSpeed());

        // Get mesh renderer component
        mono_render::ComponentMeshRenderer* meshRenderer
        = riaecs::GetComponentWithCheck<mono_render::ComponentMeshRenderer>(
            ecsWorld, entity, mono_render::ComponentMeshRendererID(), "MeshRenderer", RIAECS_LOG_LOC);

        // Get material handle manager
        mono_render::MaterialHandleManager& material_handle_manager
            = mono_render::MaterialHandleManager::GetInstance();

        // Set normal albedo texture
        std::vector<const render_graph::MaterialHandle*> material_handles;
        material_handles.resize(meshRenderer->GetMaterialHandles().size());
            for (int i = 0; i < material_handles.size(); ++i)
                material_handles[i] = material_handle_manager.GetMaterialHandle(player->GetNormalMaterialHandleID());
        meshRenderer->SetMaterialHandles(material_handles);
    }

    // Iterate through all player enhanced state components
    for (const riaecs::Entity &entity : ecsWorld.View(bdc_player::ComponentPlayerEnhancedStateID())())
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

        // Get object controller component
        mono_object_controller::ComponentObjectController* controller
        = riaecs::GetComponentWithCheck<mono_object_controller::ComponentObjectController>(
            ecsWorld, entity, mono_object_controller::ComponentObjectControllerID(), "ObjectController", RIAECS_LOG_LOC);

        // Get transform component
        mono_transform::ComponentTransform* transform
        = riaecs::GetComponentWithCheck<mono_transform::ComponentTransform>(
            ecsWorld, entity, mono_transform::ComponentTransformID(), "Transform", RIAECS_LOG_LOC);

        // Set enhanced move speed
        controller->SetMoveSpeed(player->GetEnhancedMoveSpeed());

        // Get mesh renderer component
        mono_render::ComponentMeshRenderer* meshRenderer
        = riaecs::GetComponentWithCheck<mono_render::ComponentMeshRenderer>(
            ecsWorld, entity, mono_render::ComponentMeshRendererID(), "MeshRenderer", RIAECS_LOG_LOC);

        // Get material handle manager
        mono_render::MaterialHandleManager& material_handle_manager
            = mono_render::MaterialHandleManager::GetInstance();

        // Set enhanced albedo texture
        std::vector<const render_graph::MaterialHandle*> material_handles;
        material_handles.resize(meshRenderer->GetMaterialHandles().size());
            for (int i = 0; i < material_handles.size(); ++i)
                material_handles[i] = material_handle_manager.GetMaterialHandle(player->GetEnhancedMaterialHandleID());
        meshRenderer->SetMaterialHandles(material_handles);

        // Get handler window entity from player component
        riaecs::Entity handlerWindowEntity = player->GetHandlerWindowEntity();

         // Get window component
        mono_d3d12::ComponentWindowD3D12* window
        = riaecs::GetComponentWithCheck<mono_d3d12::ComponentWindowD3D12>(
            ecsWorld, handlerWindowEntity, mono_d3d12::ComponentWindowD3D12ID(), "WindowD3D12", RIAECS_LOG_LOC);
            
        // Get input states
        const mono_input_monitor::KeyboardInputState &keyboardState = window->GetKeyboardState();
        const mono_input_monitor::MouseInputState &mouseState = window->GetMouseState();

        // Check for ability A key press
        if (mono_input_monitor::GetButtonUp(mouseState, player->GetAbilityAKey()) && !player->IsBlinking())
        {
            // Start blinking
            player->SetIsBlinking(true);

            // Calculate blink direction based on current input
            XMFLOAT3 blinkDirection = XMFLOAT3(0.0f, 0.0f, 0.0f);

            // Set blink direction and reset blinked distance
            player->SetBlinkDirection(controller->GetMoveDirection());
            player->SetCurrentBlinkedDistance(0.0f);
        }

        if (player->IsBlinking())
        {
            // Move player in blink direction
            float deltaDistance = player->GetBlinkSpeed() * deltaTimeProvider_.GetDeltaTime();
            float newBlinkedDistance = player->GetCurrentBlinkedDistance() + deltaDistance;

            // Check if blink distance exceeded
            if (newBlinkedDistance >= player->GetBlinkDistance())
            {
                deltaDistance = player->GetBlinkDistance() - player->GetCurrentBlinkedDistance();
                player->SetIsBlinking(false); // Stop blinking
            }

            // Update blinked distance
            player->SetCurrentBlinkedDistance(player->GetCurrentBlinkedDistance() + deltaDistance);

            // Calculate new position
            XMFLOAT3 currentPosition = transform->GetPos();
            XMFLOAT3 blinkDirection = player->GetBlinkDirection();
            XMFLOAT3 newPosition = XMFLOAT3(
                currentPosition.x + blinkDirection.x * deltaDistance,
                currentPosition.y,
                currentPosition.z + blinkDirection.z * deltaDistance
            );

            // Set new position
            transform->SetPos(newPosition, ecsWorld);
        }
    }

    return true; // Continue the system loop
}

BDC_PLAYER_API riaecs::SystemFactoryRegistrar<bdc_player::SystemPlayer> bdc_player::SystemPlayerID;