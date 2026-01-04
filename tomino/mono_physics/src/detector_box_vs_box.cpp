#include "mono_physics/src/pch.h"
#include "mono_physics/include/detector_box_vs_box.h"

#pragma comment(lib, "riaecs.lib")
#pragma comment(lib, "mono_transform.lib")
using namespace DirectX;

#include "mono_physics/include/component_box_collider.h"
#include "mono_physics/include/shape.h"
#include "mono_physics/include/shape_utils.h"

bool mono_physics::DetectorBoxVsBox::DetectCollisions(
    const riaecs::Entity& entityA, Collider& colliderA, mono_transform::ComponentTransform& transformA,
    const riaecs::Entity& entityB, Collider& colliderB, mono_transform::ComponentTransform& transformB)
{
    // Cast to box shapes
    const ShapeBox &boxA = static_cast<const ShapeBox&>(colliderA.GetShape());
    const ShapeBox &boxB = static_cast<const ShapeBox&>(colliderB.GetShape());

    // Check for intersection
    if (IsBoxIntersectBox(boxA, transformA.GetWorldMatrixNoRot(), boxB, transformB.GetWorldMatrixNoRot()))
    {
        // Get velocities
        XMFLOAT3 velocityA = XMFLOAT3(
            transformA.GetPos().x - transformA.GetLastPos().x,
            transformA.GetPos().y - transformA.GetLastPos().y,
            transformA.GetPos().z - transformA.GetLastPos().z);
        XMVECTOR velocityAVec = XMLoadFloat3(&velocityA);
        
        XMFLOAT3 velocityB = XMFLOAT3(
            transformB.GetPos().x - transformB.GetLastPos().x,
            transformB.GetPos().y - transformB.GetLastPos().y,
            transformB.GetPos().z - transformB.GetLastPos().z);
        XMVECTOR velocityBVec = XMLoadFloat3(&velocityB);

        // Get runner velocity
        bool isARunner = !XMVector3Equal(velocityAVec, XMVectorZero());
        bool isBRunner = !XMVector3Equal(velocityBVec, XMVectorZero());

        // If both are not moving, no collision
        if (!isARunner && !isBRunner)
            return false;

        if (isARunner)
        {
            // Get collision normal
            XMFLOAT3 collisionNormal = GetCollisionNormalFromCollidedBoxes(
                boxA, transformA.GetWorldMatrixNoRot(),
                boxB, transformB.GetWorldMatrixNoRot(), velocityA);

            // Set collision result for A
            CollisionResult& resultA = colliderA.GetCollisionResult();
            mono_physics::BoxCollisionResult& boxResultA = static_cast<mono_physics::BoxCollisionResult&>(resultA);
            boxResultA.SetCollided(true);
            boxResultA.AddCollidedEntity(entityB);
            boxResultA.AddCollisionNormal(collisionNormal);

            // Inverse normal for B
            XMFLOAT3 inverseNormal = XMFLOAT3(-collisionNormal.x, -collisionNormal.y, -collisionNormal.z);

            // Set collision result for B
            CollisionResult& resultB = colliderB.GetCollisionResult();
            mono_physics::BoxCollisionResult& boxResultB = static_cast<mono_physics::BoxCollisionResult&>(resultB);
            boxResultB.SetCollided(true);
            boxResultB.AddCollidedEntity(entityA);
            boxResultB.AddCollisionNormal(inverseNormal);
        }
        else if (isBRunner)
        {
            // Get collision normal
            XMFLOAT3 collisionNormal = GetCollisionNormalFromCollidedBoxes(
                boxB, transformB.GetWorldMatrixNoRot(),
                boxA, transformA.GetWorldMatrixNoRot(), velocityB);

            // Set collision result for B
            CollisionResult& resultB = colliderB.GetCollisionResult();
            mono_physics::BoxCollisionResult& boxResultB = static_cast<mono_physics::BoxCollisionResult&>(resultB);
            boxResultB.SetCollided(true);
            boxResultB.AddCollidedEntity(entityA);
            boxResultB.AddCollisionNormal(collisionNormal);

            // Inverse normal for A
            XMFLOAT3 inverseNormal = XMFLOAT3(-collisionNormal.x, -collisionNormal.y, -collisionNormal.z);

            // Set collision result for A
            CollisionResult& resultA = colliderA.GetCollisionResult();
            mono_physics::BoxCollisionResult& boxResultA = static_cast<mono_physics::BoxCollisionResult&>(resultA);
            boxResultA.SetCollided(true);
            boxResultA.AddCollidedEntity(entityB);
            boxResultA.AddCollisionNormal(inverseNormal);
        }
    }
    else
    {
        return false; // No collision
    }

    return true;
}