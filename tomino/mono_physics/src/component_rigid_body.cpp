#include "mono_physics/src/pch.h"
#include "mono_physics/include/component_rigid_body.h"

mono_physics::ComponentRigidBody::ComponentRigidBody()
{
}

mono_physics::ComponentRigidBody::~ComponentRigidBody()
{
    isStatic_ = false;
    mass_ = 1.0f;
    staticFriction_ = 0.5f;
    dynamicFriction_ = 0.5f;
    velocity_ = {0.0f, 0.0f, 0.0f};
}

void mono_physics::ComponentRigidBody::Setup(SetupParam &param)
{
    isStatic_ = param.isStatic;
    mass_ = param.mass;
    staticFriction_ = param.staticFriction;
    dynamicFriction_ = param.dynamicFriction;
}

void mono_physics::ComponentRigidBody::SetAttachedColliderComponentID(size_t colliderComponentID)
{
    attachedColliderComponentID_ = colliderComponentID;
    colliderAttached_ = true;
}

void mono_physics::ComponentRigidBody::RemoveAttachedColliderComponentID()
{
    attachedColliderComponentID_ = 0;
    colliderAttached_ = false;
}

bool mono_physics::ComponentRigidBody::GetAttachedColliderComponentID(size_t &colliderComponentID) const
{
    if (!colliderAttached_)
        return false;

    colliderComponentID = attachedColliderComponentID_;
    return true;
}

MONO_PHYSICS_API riaecs::ComponentRegistrar
<mono_physics::ComponentRigidBody, mono_physics::ComponentRigidBodyMaxCount> mono_physics::ComponentRigidBodyID;