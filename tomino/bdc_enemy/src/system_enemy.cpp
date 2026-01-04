#include "bdc_enemy/src/pch.h"
#include "bdc_enemy/include/system_enemy.h"

using namespace DirectX;

#include "bdc_enemy/include/component_enemy.h"

#pragma comment(lib, "riaecs.lib")
#pragma comment(lib, "mono_transform.lib")
#pragma comment(lib, "mono_delta_time.lib")

bdc_enemy::SystemEnemy::SystemEnemy()
{
}

bdc_enemy::SystemEnemy::~SystemEnemy()
{
}

bool bdc_enemy::SystemEnemy::Update
(
    riaecs::IECSWorld &ecsWorld, 
    riaecs::IAssetContainer &assetCont, 
    riaecs::ISystemLoopCommandQueue &systemLoopCmdQueue
){
    // Update delta time
    deltaTimeProvider_.UpdateTime();
    float deltaTime = deltaTimeProvider_.GetDeltaTime();

    for (const riaecs::Entity &entity : ecsWorld.View(bdc_enemy::ComponentEnemyID())())
    {
        bdc_enemy::ComponentEnemy* enemy
        = riaecs::GetComponentWithCheck<bdc_enemy::ComponentEnemy>(
            ecsWorld, entity, bdc_enemy::ComponentEnemyID(), "ComponentEnemy", RIAECS_LOG_LOC);

        // Get Transform component
        mono_transform::ComponentTransform* transform
        = riaecs::GetComponentWithCheck<mono_transform::ComponentTransform>(
            ecsWorld, entity, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

        if (enemy->GetWayPoints().empty())
            continue; // No waypoints to move to

        // Get current target waypoint
        int goingIndex = enemy->GetCurrentGoalWayPointIndex();
        const DirectX::XMFLOAT3& targetPos = enemy->GetWayPoints()[goingIndex];

        // Move towards the target waypoint
        const DirectX::XMFLOAT3& currentPos = transform->GetPos();
        DirectX::XMFLOAT3 direction{
            targetPos.x - currentPos.x, targetPos.y - currentPos.y, targetPos.z - currentPos.z};

        // Get length of direction
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
        if (length < 0.1f)
        {
            // Reached the waypoint, switch to next waypoint
            goingIndex = (goingIndex + 1) % static_cast<int>(enemy->GetWayPoints().size());

            // Update the enemy's going waypoint index
            enemy->SetCurrentGoalWayPointIndex(goingIndex);
        }
        else
        {
            // Normalize direction
            direction.x /= length;
            direction.y /= length;
            direction.z /= length;

            const float& moveSpeed = enemy->GetMoveSpeed();

            // Update position
            DirectX::XMFLOAT3 newPos{
                currentPos.x + direction.x * moveSpeed * deltaTime,
                currentPos.y + direction.y * moveSpeed * deltaTime,
                currentPos.z + direction.z * moveSpeed * deltaTime
            };
            transform->SetPos(newPos, ecsWorld);
        }

        // Rotate to face the target waypoint
        DirectX::XMFLOAT3 lookDir{
            targetPos.x - currentPos.x, 
            targetPos.y - currentPos.y, 
            targetPos.z - currentPos.z
        };
        float yaw = XMConvertToDegrees(std::atan2(lookDir.x, lookDir.z));
        transform->SetRotFromEuler(0.0f, yaw, 0.0f, ecsWorld);
    }

    return true; // Continue running
}

BDC_ENEMY_API riaecs::SystemFactoryRegistrar
<bdc_enemy::SystemEnemy> bdc_enemy::SystemEnemyID;
