#include "wbp_collision/src/pch.h"
#include "wbp_collision/include/collision_pass_box.h"

#include "wbp_collision/include/component_collision_result.h"
#include "wbp_collision/include/component_box_collider.h"

#include "wbp_primitive/include/primitive_helpers.h"
#include "wbp_primitive/include/grid.h"
#pragma comment(lib, "wbp_primitive.lib")

#include <DirectXMath.h>
using namespace DirectX;

#include "wbp_transform/plugin.h"
#pragma comment(lib, "wbp_transform.lib")

#include "wbp_identity/plugin.h"
#pragma comment(lib, "wbp_identity.lib")

void wbp_collision::CollisionPassBox::Execute(const wb::SystemArgument &args)
{
    // Get containers to use
    wb::IAssetContainer &assetContainer = args.containerStorage_.GetContainer<wb::IAssetContainer>();

    std::vector<std::unique_ptr<wb::IOptionalValue>> runnerEntityIDs;
    std::vector<std::unique_ptr<wb::IOptionalValue>> receiverEntityIDs;
    float gridSize = -FLT_MAX;
    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(wbp_collision::BoxColliderComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue; // Skip if entity is null

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), args.componentContainer_);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        if (identity == nullptr)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {
                    "Entity does not have IdentityComponent.",
                    "BoxColliderComponent requires IdentityComponent to be set.",
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

        wb::IComponent *boxColliderComponent = entity->GetComponent(wbp_collision::BoxColliderComponentID(), args.componentContainer_);
        wbp_collision::IBoxColliderComponent *boxCollider = wb::As<wbp_collision::IBoxColliderComponent>(boxColliderComponent);
        
        if (!boxCollider->IsCollisionEnabled())
        {
            // Skip if collision is not enabled
            continue;
        }

        wb::IComponent *transformComponent = entity->GetComponent(wbp_transform::TransformComponentID(), args.componentContainer_);
        wbp_transform::ITransformComponent *transform = wb::As<wbp_transform::ITransformComponent>(transformComponent);
        if (transform == nullptr)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {
                    "Entity does not have TransformComponent.",
                    "BoxColliderComponent requires TransformComponent to be set.",
                }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_COLLISION", err);
            wb::ThrowRuntimeError(err);
        }

        if (boxCollider->IsAlwaysRunnerEnabled())
        {
            runnerEntityIDs.emplace_back(id->Clone());

            // Calculate grid size based on the runner' AABBs
            for (const wbp_primitive::PrimitiveAABB &aabb : boxCollider->GetAABBs(assetContainer))
            {
                if (aabb.GetSize().x > gridSize) gridSize = aabb.GetSize().x;
                if (aabb.GetSize().y > gridSize) gridSize = aabb.GetSize().y;
                if (aabb.GetSize().z > gridSize) gridSize = aabb.GetSize().z;
            }

            continue; // Skip to next entity if always runner
        }

        if (!transform->WasDirty() || !boxCollider->IsCanBeRunnerEnabled())
        {
            receiverEntityIDs.emplace_back(id->Clone());
        }
        else
        {
            runnerEntityIDs.emplace_back(id->Clone());

            // Calculate grid size based on the runner' AABBs
            for (const wbp_primitive::PrimitiveAABB &aabb : boxCollider->GetAABBs(assetContainer))
            {
                if (aabb.GetSize().x > gridSize) gridSize = aabb.GetSize().x;
                if (aabb.GetSize().y > gridSize) gridSize = aabb.GetSize().y;
                if (aabb.GetSize().z > gridSize) gridSize = aabb.GetSize().z;
            }
        }
    }

    // If no runners or receivers, exit early
    if (runnerEntityIDs.empty() || receiverEntityIDs.empty())
    {
        return;
    }

    // Uniform grid
    wbp_primitive::SpatialGrid spatialGrid(gridSize);

    // Register AABBs for each runner
    std::vector<wbp_primitive::PrimitiveAABB> runnerMovementAABBs;
    std::vector<XMFLOAT3> runnerMovementVecs;
    for (size_t i = 0; i < runnerEntityIDs.size(); i++)
    {
        wb::IEntity *runnerEntity = args.entityContainer_.PtrGet(*runnerEntityIDs[i]);
        if (runnerEntity == nullptr) continue; // Skip if runner entity is null

        wb::IComponent *runnerTransformComponent = runnerEntity->GetComponent(wbp_transform::TransformComponentID(), args.componentContainer_);
        wbp_transform::ITransformComponent *runnerTransform = wb::As<wbp_transform::ITransformComponent>(runnerTransformComponent);

        wb::IComponent *runnerBoxColliderComponent = runnerEntity->GetComponent(wbp_collision::BoxColliderComponentID(), args.componentContainer_);
        wbp_collision::IBoxColliderComponent *runnerBoxCollider = wb::As<wbp_collision::IBoxColliderComponent>(runnerBoxColliderComponent);

        // Create total AABB from the runner's box collider
        wbp_primitive::PrimitiveAABB totalAABB = wbp_primitive::CreateAABBFromAABBs
        (
            runnerBoxCollider->GetAABBs(assetContainer), XMMatrixIdentity()
        );

        // Create movement vector based on runnerTransform
        DirectX::XMFLOAT3 movementVec = 
        {
            runnerTransform->GetPosition().x - runnerTransform->GetPreviousPosition().x,
            runnerTransform->GetPosition().y - runnerTransform->GetPreviousPosition().y,
            runnerTransform->GetPosition().z - runnerTransform->GetPreviousPosition().z
        };

        // Create AABB from movement vector
        wbp_primitive::PrimitiveAABB movementAABB = wbp_primitive::CreateAABBFromAABBMovement
        (
            totalAABB, 
            runnerTransform->GetPreviousWorldMatrixWithoutRot(), 
            runnerTransform->GetWorldMatrixWithoutRot()
        );
        
        spatialGrid.RegisterAABB(i, movementAABB);

        runnerMovementAABBs.emplace_back(std::move(movementAABB));
        runnerMovementVecs.emplace_back(std::move(movementVec));
    }
    
    // Register AABBs for each receiver
    std::vector<std::unique_ptr<wb::IOptionalValue>> receiverAABBsIDs;
    std::vector<wbp_primitive::PrimitiveAABB> receiverAABBs;
    size_t receiverAABBCount = 0;
    for (size_t i = 0; i < receiverEntityIDs.size(); i++)
    {
        wb::IEntity *receiverEntity = args.entityContainer_.PtrGet(*receiverEntityIDs[i]);
        if (receiverEntity == nullptr) continue; // Skip if receiver entity is null

        wb::IComponent *receiverTransformComponent = receiverEntity->GetComponent(wbp_transform::TransformComponentID(), args.componentContainer_);
        wbp_transform::ITransformComponent *receiverTransform = wb::As<wbp_transform::ITransformComponent>(receiverTransformComponent);

        wb::IComponent *receiverBoxColliderComponent = receiverEntity->GetComponent(wbp_collision::BoxColliderComponentID(), args.componentContainer_);
        wbp_collision::IBoxColliderComponent *receiverBoxCollider = wb::As<wbp_collision::IBoxColliderComponent>(receiverBoxColliderComponent);

        // Register AABBs for the receiver
        for (const wbp_primitive::PrimitiveAABB &aabb : receiverBoxCollider->GetAABBs(assetContainer))
        {
            XMVECTOR aabbMin = XMVector3TransformCoord(aabb.GetMinVec(), receiverTransform->GetWorldMatrixWithoutRot());
            XMVECTOR aabbMax = XMVector3TransformCoord(aabb.GetMaxVec(), receiverTransform->GetWorldMatrixWithoutRot());
            wbp_primitive::PrimitiveAABB transformedAABB(aabbMin, aabbMax);

            spatialGrid.RegisterAABB(runnerEntityIDs.size() + receiverAABBCount, transformedAABB);

            receiverAABBsIDs.emplace_back(receiverEntity->GetID().Clone());
            receiverAABBs.emplace_back(transformedAABB);

            receiverAABBCount++;
        }
    }

    for (size_t i = 0; i < runnerEntityIDs.size(); i++)
    {
        wb::IEntity *runnerEntity = args.entityContainer_.PtrGet(*runnerEntityIDs[i]);

        wb::IComponent *runnerTransformComponent = runnerEntity->GetComponent(wbp_transform::TransformComponentID(), args.componentContainer_);
        wbp_transform::ITransformComponent *runnerTransform = wb::As<wbp_transform::ITransformComponent>(runnerTransformComponent);

        wb::IComponent *runnerBoxColliderComponent = runnerEntity->GetComponent(wbp_collision::BoxColliderComponentID(), args.componentContainer_);
        wbp_collision::IBoxColliderComponent *runnerBoxCollider = wb::As<wbp_collision::IBoxColliderComponent>(runnerBoxColliderComponent);

        wb::IComponent *runnerCollisionResultComponent = runnerEntity->GetComponent(wbp_collision::CollisionResultComponentID(), args.componentContainer_);
        wbp_collision::ICollisionResultComponent *runnerCollisionResult = wb::As<wbp_collision::ICollisionResultComponent>(runnerCollisionResultComponent);

        // Query nearby entities using spatial grid
        std::vector<size_t> nearbyEntityIDsInGrid = spatialGrid.QueryNearby(i, runnerMovementAABBs[i]);
        if (nearbyEntityIDsInGrid.empty())
        {
            // If no nearby entities, skip collision checks
            continue;
        }

        for (const size_t &nearbyEntityIDInGrid : nearbyEntityIDsInGrid)
        {
            if (nearbyEntityIDInGrid == i)
            {
                // Skip if the nearby entity is the same as the runner
                continue;
            }

            bool nearbyIsRunner = false;
            std::unique_ptr<wb::IOptionalValue> nearbyEntityID = nullptr;
            if (nearbyEntityIDInGrid < runnerEntityIDs.size())
            {
                // If the nearby entity is a runner, use its ID
                nearbyIsRunner = true;
                nearbyEntityID = runnerEntityIDs[nearbyEntityIDInGrid]->Clone();
            }
            else
            {
                // If the nearby entity is a receiver, use its ID
                nearbyIsRunner = false;
                nearbyEntityID = receiverAABBsIDs[nearbyEntityIDInGrid - runnerEntityIDs.size()]->Clone();
            }

            wb::IEntity *nearbyEntity = args.entityContainer_.PtrGet(*nearbyEntityID);

            for (const size_t &ignoreComponentID : runnerBoxCollider->GetIgnoreComponentIDs())
            {
                if (nearbyEntity->GetComponent(ignoreComponentID, args.componentContainer_) != nullptr)
                {
                    // Skip if the receiver has a component that should be ignored
                    continue;
                }
            }

            // If runner has target components, check if the receiver has them
            if (!runnerBoxCollider->GetTargetComponentIDs().empty())
            {
                for (const size_t &targetComponentID : runnerBoxCollider->GetTargetComponentIDs())
                {
                    if (nearbyEntity->GetComponent(targetComponentID, args.componentContainer_) == nullptr)
                    {
                        // Skip if the receiver does not have a target component
                        continue;
                    }
                }
            }

            bool isIntersected = false;
            if (nearbyIsRunner)
            {
                isIntersected = wbp_primitive::IntersectAABBs
                (
                    runnerMovementAABBs[i], 
                    XMMatrixIdentity(),
                    runnerMovementAABBs[nearbyEntityIDInGrid], 
                    XMMatrixIdentity()
                );
            }
            else
            {
                isIntersected = wbp_primitive::IntersectAABBs
                (
                    runnerMovementAABBs[i], 
                    XMMatrixIdentity(),
                    receiverAABBs[nearbyEntityIDInGrid - runnerEntityIDs.size()], 
                    XMMatrixIdentity()
                );
            }

            if (!isIntersected)
            {
                // Skip if AABBs do not intersect
                continue;
            }

            if (runnerCollisionResult != nullptr)
            {
                XMFLOAT3 collidedFaceNormal;
                if (nearbyIsRunner)
                {
                    collidedFaceNormal = wbp_primitive::GetCollidedFaceNormal
                    (
                        runnerMovementAABBs[nearbyEntityIDInGrid], XMMatrixIdentity(),
                        runnerMovementAABBs[i], XMMatrixIdentity(),
                        runnerMovementVecs[i]
                    );
                }
                else
                {
                    collidedFaceNormal = wbp_primitive::GetCollidedFaceNormal
                    (
                        receiverAABBs[nearbyEntityIDInGrid - runnerEntityIDs.size()], 
                        XMMatrixIdentity(),
                        runnerMovementAABBs[i], XMMatrixIdentity(),
                        runnerMovementVecs[i]
                    );
                }

                wb::IComponent *nearbyBoxColliderComponent = nearbyEntity->GetComponent(wbp_collision::BoxColliderComponentID(), args.componentContainer_);
                wbp_collision::IBoxColliderComponent *nearbyBoxCollider = wb::As<wbp_collision::IBoxColliderComponent>(nearbyBoxColliderComponent);

                // Add collision result
                runnerCollisionResult->AddCollided
                (
                    nearbyEntity->GetID().Clone(),
                    collidedFaceNormal, nearbyBoxCollider->IsTrigger()
                );
            }

            wb::IComponent *nearbyCollisionResultComponent = nearbyEntity->GetComponent(wbp_collision::CollisionResultComponentID(), args.componentContainer_);
            wbp_collision::ICollisionResultComponent *nearbyCollisionResult = wb::As<wbp_collision::ICollisionResultComponent>(nearbyCollisionResultComponent);
            if (nearbyCollisionResult != nullptr)
            {
                XMFLOAT3 collidedFaceNormal;
                if (nearbyIsRunner)
                {
                    collidedFaceNormal = wbp_primitive::GetCollidedFaceNormal
                    (
                        runnerMovementAABBs[nearbyEntityIDInGrid], XMMatrixIdentity(),
                        runnerMovementAABBs[i], XMMatrixIdentity(),
                        runnerMovementVecs[i]
                    );
                }
                else
                {
                    collidedFaceNormal = wbp_primitive::GetCollidedFaceNormal
                    (
                        receiverAABBs[nearbyEntityIDInGrid - runnerEntityIDs.size()], 
                        XMMatrixIdentity(),
                        runnerMovementAABBs[i], XMMatrixIdentity(),
                        runnerMovementVecs[i]
                    );
                }

                // This face is the face of the nearby entity, so invert the normal
                collidedFaceNormal = XMFLOAT3(-collidedFaceNormal.x, -collidedFaceNormal.y, -collidedFaceNormal.z);

                wb::IComponent *nearbyBoxColliderComponent = nearbyEntity->GetComponent(wbp_collision::BoxColliderComponentID(), args.componentContainer_);
                wbp_collision::IBoxColliderComponent *nearbyBoxCollider = wb::As<wbp_collision::IBoxColliderComponent>(nearbyBoxColliderComponent);

                // Add collision result
                nearbyCollisionResult->AddCollided
                (
                    runnerEntity->GetID().Clone(),
                    collidedFaceNormal, nearbyBoxCollider->IsTrigger()
                );
            }
        }
    }
}