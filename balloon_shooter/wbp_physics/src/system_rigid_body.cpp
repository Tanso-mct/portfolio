#include "wbp_physics/src/pch.h"
#include "wbp_physics/include/system_rigid_body.h"

#include "wbp_physics/include/component_rigid_body.h"

using namespace DirectX;

#include "wbp_identity/plugin.h"
#pragma comment(lib, "wbp_identity.lib")

#include "wbp_transform/plugin.h"
#pragma comment(lib, "wbp_transform.lib")

#include "wbp_collision/plugin.h"
#pragma comment(lib, "wbp_collision.lib")

const WBP_PHYSICS_API size_t &wbp_physics::RigidBodySystemID()
{
    static size_t id = wb::IDFactory::CreateSystemID();
    return id;
}

const size_t &wbp_physics::RigidBodySystem::GetID() const
{
    return wbp_physics::RigidBodySystemID();

}
void wbp_physics::RigidBodySystem::Initialize(wb::IAssetContainer &assetContainer)
{
}

void wbp_physics::RigidBodySystem::Update(const wb::SystemArgument &args)
{
    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(wbp_physics::RigidBodyComponentID()))
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
                    "RigidBodyComponent requires IdentityComponent to be set."
                }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_PHYSICS", err);
            wb::ThrowRuntimeError(err);
        }

        if (identity->IsActiveSelf() == false)
        {
            // Skip if entity is not active
            continue;
        }

        wb::IComponent *rigidBodyComponent = entity->GetComponent(wbp_physics::RigidBodyComponentID(), args.componentContainer_);
        wbp_physics::IRigidBodyComponent *rigidBody = wb::As<wbp_physics::IRigidBodyComponent>(rigidBodyComponent);

        if (rigidBody->IsKinematic())
        {
            if (rigidBody->GetVelocity().x == 0.0f && rigidBody->GetVelocity().y == 0.0f && rigidBody->GetVelocity().z == 0.0f)
            {
                // Skip if velocity is zero
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
                        "RigidBodyComponent requires TransformComponent to be set."
                    }
                );
                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WBP_PHYSICS", err);
                wb::ThrowRuntimeError(err);
            }

            rigidBody->SetInitialPos(transform->GetLocalPosition());

            // Get new local position based on velocity
            XMVECTOR newLocalPosVec 
                = XMLoadFloat3(&transform->GetLocalPosition()) + XMLoadFloat3(&rigidBody->GetVelocity());

            XMFLOAT3 newLocalPos;
            XMStoreFloat3(&newLocalPos, newLocalPosVec);

            rigidBody->SetTerminalPos(newLocalPos);

            // Update local position
            transform->SetLocalPosition(newLocalPos);
        }
    }
}

namespace wbp_physics
{
    WB_REGISTER_SYSTEM(RigidBodySystem, RigidBodySystemID());

} // namespace wbp_physics

const WBP_PHYSICS_API size_t &wbp_physics::RigidBodyResponseSystemID()
{
    static size_t id = wb::IDFactory::CreateSystemID();
    return id;
}

const size_t &wbp_physics::RigidBodyResponseSystem::GetID() const
{
    return wbp_physics::RigidBodyResponseSystemID();
}

void wbp_physics::RigidBodyResponseSystem::Initialize(wb::IAssetContainer &assetContainer)
{
}

void wbp_physics::RigidBodyResponseSystem::Update(const wb::SystemArgument &args)
{
    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(wbp_physics::RigidBodyComponentID()))
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
                    "RigidBodyComponent requires IdentityComponent to be set."
                }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_PHYSICS", err);
            wb::ThrowRuntimeError(err);
        }

        if (identity->IsActiveSelf() == false)
        {
            // Skip if entity is not active
            continue;
        }

        wb::IComponent *rigidBodyComponent = entity->GetComponent(wbp_physics::RigidBodyComponentID(), args.componentContainer_);
        wbp_physics::IRigidBodyComponent *rigidBody = wb::As<wbp_physics::IRigidBodyComponent>(rigidBodyComponent);

        if (!rigidBody->IsAffectedByOther())
        {
            // Skip if not affected by other entities
            continue;
        }

        if (rigidBody->IsKinematic())
        {
            wb::IComponent *collisionResultComponent = entity->GetComponent(wbp_collision::CollisionResultComponentID(), args.componentContainer_);
            wbp_collision::ICollisionResultComponent *collisionResult = wb::As<wbp_collision::ICollisionResultComponent>(collisionResultComponent);
            if (collisionResult == nullptr)
            {
                // Skip if collision result component is not set
                continue;
            }

            if (!collisionResult->IsCollided())
            {
                // Skip if not collided
                continue;
            }

            // Get movement vector from velocity
            XMVECTOR movementVec = XMLoadFloat3(&rigidBody->GetVelocity());

            for (size_t i = 0; i < collisionResult->GetCollidedCount(); ++i)
            {
                wb::IEntity *collidedEntity = args.entityContainer_.PtrGet(collisionResult->GetCollidedEntityID(i));
                if (collidedEntity == nullptr)
                {
                    // Skip if collided entity is null
                    continue;
                }

                if (collisionResult->GetCollidedIsTrigger(i))
                {
                    // Skip if collided entity is a trigger
                    continue;
                }

                XMVECTOR collidedFaceNormalVec = XMLoadFloat3(&collisionResult->GetCollidedFaceNormal(i));

                // Project movement vector onto the collided face normal
                XMVECTOR projectedMovementVec = XMVector3Dot(movementVec, collidedFaceNormalVec) * collidedFaceNormalVec;

                // Calculate the response movement vector
                movementVec -= projectedMovementVec;
            }

            // Get the corrected LocalPosition
            XMVECTOR newLocalPosVec = XMLoadFloat3(&rigidBody->GetInitialPos()) + movementVec;
            XMFLOAT3 newLocalPos;
            XMStoreFloat3(&newLocalPos, newLocalPosVec);

            wb::IComponent *transformComponent = entity->GetComponent(wbp_transform::TransformComponentID(), args.componentContainer_);
            wbp_transform::ITransformComponent *transform = wb::As<wbp_transform::ITransformComponent>(transformComponent);
            if (transform == nullptr)
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {
                        "RigidBodyComponent requires TransformComponent to be set."
                    }
                );
                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WBP_PHYSICS", err);
                wb::ThrowRuntimeError(err);
            }

            // Update the LocalPosition
            transform->SetLocalPosition(newLocalPos);

            // Update the terminal position
            rigidBody->SetTerminalPos(newLocalPos);
        }
    }
}

namespace wbp_physics
{
    WB_REGISTER_SYSTEM(RigidBodyResponseSystem, RigidBodyResponseSystemID());

} // namespace wbp_physics