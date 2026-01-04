#include "mono_physics/src/pch.h"
#include "mono_physics/include/collision_resolver.h"

void mono_physics::CollisionResolverRegistry::Register(
    size_t colliderComponentID, std::unique_ptr<CollisionResolver> resolver)
{
    assert(resolver != nullptr); // Ensure resolver is not null
    assert(resolvers_.find(colliderComponentID) == resolvers_.end()); // Ensure no duplicate registration

    // Register the resolver
    resolvers_[colliderComponentID] = std::move(resolver);
}

mono_physics::CollisionResolver &mono_physics::CollisionResolverRegistry::Get(size_t colliderComponentID) const
{
    assert(resolvers_.find(colliderComponentID) != resolvers_.end()); // Ensure the resolver exists
    return *resolvers_.at(colliderComponentID);
}