#include "mono_physics/src/pch.h"
#include "mono_physics/include/component_ray_collider.h"

#include "mono_physics/include/shape_utils.h"

mono_physics::RayCollisionResult::RayCollisionResult()
{
}

mono_physics::RayCollisionResult::~RayCollisionResult()
{
    collisionPoints_.clear();
    collisionDistances_.clear();

    closestEntity_ = riaecs::Entity();
    closestPoint_ = { 0.0f, 0.0f, 0.0f };
    closestDistance_ = FLT_MAX;

    furthestEntity_ = riaecs::Entity();
    furthestPoint_ = { 0.0f, 0.0f, 0.0f };
    furthestDistance_ = 0.0f;
}

void mono_physics::RayCollisionResult::Clear()
{
    // Clear base class data
    collided_ = false;
    collidedEntities_.clear();

    // Clear ray collision specific data
    collisionPoints_.clear();
    collisionDistances_.clear();

    closestEntity_ = riaecs::Entity();
    closestPoint_ = { 0.0f, 0.0f, 0.0f };
    closestDistance_ = FLT_MAX;

    furthestEntity_ = riaecs::Entity();
    furthestPoint_ = { 0.0f, 0.0f, 0.0f };
    furthestDistance_ = 0.0f;
}

void mono_physics::RayCollisionResult::SetClosestEntity(
    riaecs::Entity entity, const DirectX::XMFLOAT3 &point, float distance)
{
    closestEntity_ = entity;
    closestPoint_ = point;
    closestDistance_ = distance;
}

void mono_physics::RayCollisionResult::SetFurthestEntity(
    riaecs::Entity entity, const DirectX::XMFLOAT3 &point, float distance)
{
    furthestEntity_ = entity;
    furthestPoint_ = point;
    furthestDistance_ = distance;
}

mono_physics::ComponentRayCollider::ComponentRayCollider()
{
}

mono_physics::ComponentRayCollider::~ComponentRayCollider()
{
}

void mono_physics::ComponentRayCollider::Setup(SetupParam &param)
{
    isTrigger_ = param.isTrigger;
    collisionResult_ = std::move(param.collisionResult);

    // Update bounding box based on the ray
    boundingBox_ = mono_physics::CreateBoxFromVector(param.ray->GetDirection(), param.ray->GetOrigin());

    shape_ = std::move(param.ray);
}

const mono_physics::ShapeRay &mono_physics::ComponentRayCollider::GetRay() const
{
    return static_cast<const mono_physics::ShapeRay&>(*shape_);
}

void mono_physics::ComponentRayCollider::SetRay(std::unique_ptr<ShapeRay> ray)
{
    shape_ = std::move(ray);

    // Update bounding box based on the ray
    boundingBox_ = mono_physics::CreateBoxFromVector(GetRay().GetDirection(), GetRay().GetOrigin());
}

const mono_physics::RayCollisionResult &mono_physics::ComponentRayCollider::GetRayCollisionResult() const
{
    return static_cast<const mono_physics::RayCollisionResult&>(*collisionResult_);
}

void mono_physics::ComponentRayCollider::SetRayCollisionResult(std::unique_ptr<RayCollisionResult> result)
{
    collisionResult_ = std::move(result);
}

MONO_PHYSICS_API riaecs::ComponentRegistrar
<mono_physics::ComponentRayCollider, mono_physics::ComponentRayColliderMaxCount> mono_physics::ComponentRayColliderID;