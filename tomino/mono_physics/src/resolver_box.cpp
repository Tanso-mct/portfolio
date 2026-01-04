#include "mono_physics/src/pch.h"
#include "mono_physics/include/resolver_box.h"

#include "mono_physics/include/component_box_collider.h"

#pragma comment(lib, "riaecs.lib")
using namespace DirectX;

void mono_physics::ResolverBox::ResolveCollision(
    Collider &collider, ComponentRigidBody &rigidBody, 
    size_t resultIndex, Collider& otherCollider, ComponentRigidBody &otherRigidBody)
{
    // Get collision result
    CollisionResult &collisionResult = collider.GetCollisionResult();
    BoxCollisionResult &boxResult = static_cast<BoxCollisionResult&>(collisionResult);

    // If it is a trigger, do not resolve
    if (collider.IsTrigger())
        return;

    // If it is static, do not resolve
    if (rigidBody.IsStatic())
        return;

    // If other is a trigger, do not resolve
    if (otherCollider.IsTrigger())
        return;

    // Get collision normal
    XMFLOAT3 collisionNormal = boxResult.GetCollisionNormals()[resultIndex];
    XMVECTOR collisionNormalVec = XMLoadFloat3(&collisionNormal);

    // Get velocity
    XMVECTOR velocityVec = XMLoadFloat3(&rigidBody.GetVelocity());

    // Project velocity vector onto the collided face normal
    XMVECTOR projectedVelocityVec = XMVector3Dot(velocityVec, collisionNormalVec) * collisionNormalVec;

    // Calculate the response velocity vector
    XMVECTOR responseVelocityVec = velocityVec - projectedVelocityVec;

    // Store the response velocity
    XMFLOAT3 responseVelocity;
    XMStoreFloat3(&responseVelocity, responseVelocityVec);
    rigidBody.SetVelocity(responseVelocity);
}