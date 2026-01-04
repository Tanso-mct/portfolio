#pragma once
#include "mono_physics/include/dll_config.h"
#include "riaecs/riaecs.h"

#include "mono_physics/include/collider.h"
#include "mono_physics/include/component_rigid_body.h"
#include "mono_physics/include/collision_resolver.h"

namespace mono_physics
{
    class MONO_PHYSICS_API ResolverBox : public CollisionResolver
    {
    public:
        ResolverBox() = default;
        ~ResolverBox() = default;

        // Resolve collision between two colliders
        void ResolveCollision(
            Collider& collider, ComponentRigidBody& rigidBody, 
            size_t resultIndex, Collider& otherCollider, ComponentRigidBody& otherRigidBody) override;
    };

} // namespace mono_physics