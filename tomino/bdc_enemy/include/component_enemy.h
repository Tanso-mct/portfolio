#pragma once
#include "bdc_enemy/include/dll_config.h"
#include "riaecs/riaecs.h"

#include <DirectXMath.h>

namespace bdc_enemy
{
    constexpr size_t ComponentEnemyMaxCount = 500;
    class BDC_ENEMY_API ComponentEnemy
    {
    private:
        // Waypoints for enemy patrol or movement
        std::vector<DirectX::XMFLOAT3> wayPoints_;
        
        // Currently going waypoint index
        int goingWayPointIndex_ = 0;

        // Movement speed
        float moveSpeed_ = 0.2f;

    public:
        ComponentEnemy();
        ~ComponentEnemy();

        struct SetupParam
        {
            // Waypoints for enemy patrol or movement
            std::vector<DirectX::XMFLOAT3> wayPoints;

            // Inital going waypoint index
            int goingWayPointIndex = 0;

            // Movement speed
            float moveSpeed = 0.2f;
        };
        void Setup(SetupParam &param);

        // Get the list of waypoints
        const std::vector<DirectX::XMFLOAT3>& GetWayPoints() const { return wayPoints_; }

        // Get the currently going waypoint index
        int GetCurrentGoalWayPointIndex() const { return goingWayPointIndex_; }

        // Set the currently going waypoint index
        void SetCurrentGoalWayPointIndex(int index) { goingWayPointIndex_ = index; }

        // Get movement speed
        float GetMoveSpeed() const { return moveSpeed_; }

        // Set movement speed
        void SetMoveSpeed(float speed) { moveSpeed_ = speed; }
    };

    extern BDC_ENEMY_API riaecs::ComponentRegistrar
    <ComponentEnemy, ComponentEnemyMaxCount> ComponentEnemyID;

} // namespace bdc_enemy