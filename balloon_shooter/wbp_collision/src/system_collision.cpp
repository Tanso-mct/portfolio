#include "wbp_collision/src/pch.h"
#include "wbp_collision/include/system_collision.h"

#include "wbp_collision/include/collision_pass_box.h"
#include "wbp_collision/include/collision_pass_ray.h"

#include "wbp_collision/include/component_collision_result.h"

const WBP_COLLISION_API size_t &wbp_collision::CollisionSystemID()
{
    static size_t id = wb::IDFactory::CreateSystemID();
    return id;
}

wbp_collision::CollisionSystem::CollisionSystem()
{
    // Initialize collision passes. The order of addition to vector is the order of execution.
    passes_.clear();
    passes_.emplace_back(std::make_unique<wbp_collision::CollisionPassBox>());
    passes_.emplace_back(std::make_unique<wbp_collision::CollisionPassRay>());
}

const size_t &wbp_collision::CollisionSystem::GetID() const
{
    return CollisionSystemID();
}

void wbp_collision::CollisionSystem::Initialize(wb::IAssetContainer &assetContainer)
{

}

void wbp_collision::CollisionSystem::Update(const wb::SystemArgument &args)
{
    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(wbp_collision::CollisionResultComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue; // Skip if entity is null

        wb::IComponent *collisionResultComponent = entity->GetComponent(wbp_collision::CollisionResultComponentID(), args.componentContainer_);
        wbp_collision::ICollisionResultComponent *collisionResult = wb::As<wbp_collision::ICollisionResultComponent>(collisionResultComponent);
        if (collisionResult != nullptr)
        {
            // Clear previous collision results
            collisionResult->ClearCollided();
        }
    }

    for (const std::unique_ptr<wbp_collision::ICollisionPass> &pass : passes_)
    {
        pass->Execute(args);
    }
}

namespace wbp_collision
{
    WB_REGISTER_SYSTEM(CollisionSystem, CollisionSystemID());

} // namespace wbp_collision