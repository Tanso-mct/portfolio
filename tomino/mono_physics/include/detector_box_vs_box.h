#pragma once

#include "mono_physics/include/dll_config.h"
#include "mono_physics/include/collision_detector.h"

namespace mono_physics
{
    class MONO_PHYSICS_API DetectorBoxVsBox : public CollisionDetector
    {
    public:
        DetectorBoxVsBox() = default;
        ~DetectorBoxVsBox() override = default;

        bool DetectCollisions(
            const riaecs::Entity& entityA, Collider& colliderA, mono_transform::ComponentTransform& transformA,
            const riaecs::Entity& entityB, Collider& colliderB, mono_transform::ComponentTransform& transformB) override;
    };
    
} // namespace mono_physics