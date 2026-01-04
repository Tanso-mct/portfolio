#pragma once
#include "mono_physics/include/dll_config.h"
#include "riaecs/riaecs.h"

#include "mono_delta_time/mono_delta_time.h"
#include "mono_physics/include/grid.h"
#include "mono_physics/include/collision_detector.h"
#include "mono_physics/include/collision_resolver.h"

namespace mono_physics
{
    class MONO_PHYSICS_API SystemPhysics : public riaecs::ISystem
    {
    private:
        // Delta time provider
        mono_delta_time::DeltaTimeProvider deltaTimeProvider_ = mono_delta_time::DeltaTimeProvider();

        // Spatial grid for broadphase collision detection
        const float girdCellSize_ = 10.0f; // Size of each grid cell
        SpatialGrid spatialGrid_;

        // Registry of collision detectors
        CollisionDetectorRegistry collisionDetectorRegistry_ = CollisionDetectorRegistry();

        // Registry of collision resolvers
        CollisionResolverRegistry CollisionResolverRegistry_ = CollisionResolverRegistry();

    public:
        SystemPhysics();
        ~SystemPhysics() override;

        /***************************************************************************************************************
         * ISystem Implementation
        /**************************************************************************************************************/

        bool Update
        (
            riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
            riaecs::ISystemLoopCommandQueue &systemLoopCmdQueue
        ) override;
    };
    extern MONO_PHYSICS_API riaecs::SystemFactoryRegistrar<SystemPhysics> SystemPhysicsID;

} // namespace mono_physics