#include "wbp_collision/src/pch.h"
#include "wbp_collision/include/collision_pass_ray.h"

#include "wbp_collision/include/component_collision_result.h"
#include "wbp_collision/include/component_box_collider.h"
#include "wbp_collision/include/component_ray_collider.h"

#include "wbp_primitive/include/primitive_helpers.h"
#pragma comment(lib, "wbp_primitive.lib")

#include <DirectXMath.h>
using namespace DirectX;

#include "wbp_identity/plugin.h"
#pragma comment(lib, "wbp_identity.lib")

#include "wbp_transform/plugin.h"
#pragma comment(lib, "wbp_transform.lib")

void wbp_collision::CollisionPassRay::Execute(const wb::SystemArgument &args)
{
    // Get containers to use
    wb::IAssetContainer &assetContainer = args.containerStorage_.GetContainer<wb::IAssetContainer>();

    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(wbp_collision::RayColliderComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue; // Skip if entity is null

        wb::IComponent *collisionResultComponent = entity->GetComponent(wbp_collision::CollisionResultComponentID(), args.componentContainer_);
        wbp_collision::ICollisionResultComponent *collisionResult = wb::As<wbp_collision::ICollisionResultComponent>(collisionResultComponent);
        if (collisionResult == nullptr)
        {
            // Skip if no collision result component
            continue;
        }

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), args.componentContainer_);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        if (identity == nullptr)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {
                    "RayColliderComponent requires IdentityComponent to be set.",
                }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_COLLISION", err);
            wb::ThrowRuntimeError(err);
        }

        if (!identity->IsActiveSelf())
        {
            // Skip if the entity is not active
            continue;
        }

        wb::IComponent *rayColliderComponent = entity->GetComponent(wbp_collision::RayColliderComponentID(), args.componentContainer_);
        wbp_collision::IRayColliderComponent *rayCollider = wb::As<wbp_collision::IRayColliderComponent>(rayColliderComponent);

        if (!rayCollider->IsCollisionEnabled())
        {
            // Skip if collision is not enabled
            continue;
        }

        if (!(rayCollider->IsCast() || rayCollider->IsCasting()))
        {
            // Skip if the ray is not casted or casting
            continue;
        }

        // Reset the cast flag
        rayCollider->SetCast(false);

        // Create AABB from the ray
        wbp_primitive::PrimitiveAABB rayAABB = wbp_primitive::CreateAABBFromRay(rayCollider->GetRay());

        for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(wbp_collision::BoxColliderComponentID()))
        {
            wb::IEntity *colliderEntity = args.entityContainer_.PtrGet(*id);
            if (colliderEntity == nullptr) continue; // Skip if collider entity is null

			bool needsSkip = true;
            for (const size_t &targetComponentID : rayCollider->GetTargetComponentIDs())
            {
                if (colliderEntity->GetComponent(targetComponentID, args.componentContainer_) == nullptr)
                {
                    // Skip if the collider entity does not have the target component
                    continue;
                }

				needsSkip = false; // Found a target component, do not skip
            }

            if (needsSkip)
            {
                continue; // Skip this collider entity
			}

            wb::IComponent *boxColliderComponent = colliderEntity->GetComponent(wbp_collision::BoxColliderComponentID(), args.componentContainer_);
            wbp_collision::IBoxColliderComponent *boxCollider = wb::As<wbp_collision::IBoxColliderComponent>(boxColliderComponent);

            wb::IComponent *transformComponent = colliderEntity->GetComponent(wbp_transform::TransformComponentID(), args.componentContainer_);
            wbp_transform::ITransformComponent *transform = wb::As<wbp_transform::ITransformComponent>(transformComponent);
            if (transform == nullptr)
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {
                        "BoxColliderComponent requires TransformComponent to be set.",
                    }
                );
                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WBP_COLLISION", err);
                wb::ThrowRuntimeError(err);
            }

            for (const wbp_primitive::PrimitiveAABB &aabb : boxCollider->GetAABBs(assetContainer))
            {
                // Check if the ray intersects with the AABB
                float tmin = 0.0f;
                float tmax = 0.0f;
                if 
                (
                    wbp_primitive::IntersectRayAABB
                    (
                        rayCollider->GetRay(), aabb, transform->GetWorldMatrixWithoutRot(),
                        &tmin, &tmax
                    )
                ){
                    std::vector<XMFLOAT3> hitPoints;
                    hitPoints.emplace_back
                    (
                        XMFLOAT3
                        (
                            rayCollider->GetRay().GetOrigin().x + rayCollider->GetRay().GetDirection().x * tmin,
                            rayCollider->GetRay().GetOrigin().y + rayCollider->GetRay().GetDirection().y * tmin,
                            rayCollider->GetRay().GetOrigin().z + rayCollider->GetRay().GetDirection().z * tmin
                        )
                    );

                    hitPoints.emplace_back
                    (
                        XMFLOAT3
                        (
                            rayCollider->GetRay().GetOrigin().x + rayCollider->GetRay().GetDirection().x * tmax,
                            rayCollider->GetRay().GetOrigin().y + rayCollider->GetRay().GetDirection().y * tmax,
                            rayCollider->GetRay().GetOrigin().z + rayCollider->GetRay().GetDirection().z * tmax
                        )
                    );

                    collisionResult->AddCollided
                    (
                        colliderEntity->GetID().Clone(),
                        std::move(hitPoints), { tmin, tmax },
                        boxCollider->IsTrigger()
                    );
                }
            }
        }
    }
}