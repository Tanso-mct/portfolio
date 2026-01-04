#pragma once

#include <utility>
#include <memory>
#include <unordered_map>

#include "mono_transform/mono_transform.h"

#include "mono_physics/include/dll_config.h"
#include "mono_physics/include/collider.h"
#include "mono_physics/include/component_rigid_body.h"

namespace mono_physics
{
    class CollisionResolver
    {
    public:
        CollisionResolver() = default;
        ~CollisionResolver() = default;

        // Resolve collision between two colliders
        virtual void ResolveCollision(
            Collider& collider, ComponentRigidBody& rigidBody, 
            size_t resultIndex, Collider& otherCollider, ComponentRigidBody& otherRigidBody) = 0;
    };

    class MONO_PHYSICS_API CollisionResolverRegistry
    {
    public:
        CollisionResolverRegistry() = default;
        ~CollisionResolverRegistry() = default;

        // Delete copy constructor and assignment operator
        CollisionResolverRegistry(const CollisionResolverRegistry&) = delete;
        CollisionResolverRegistry& operator=(const CollisionResolverRegistry&) = delete;

        void Register(size_t colliderComponentID, std::unique_ptr<CollisionResolver> resolver);
        CollisionResolver& Get(size_t colliderComponentID) const;

    private:
        std::unordered_map<size_t, std::unique_ptr<CollisionResolver>> resolvers_;
    };

} // namespace mono_physics