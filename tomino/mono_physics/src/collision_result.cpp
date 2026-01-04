#include "mono_physics/src/pch.h"
#include "mono_physics/include/collision_result.h"

mono_physics::CollisionResult::CollisionResult()
{
}

mono_physics::CollisionResult::~CollisionResult()
{
}

void mono_physics::CollisionResult::Clear()
{
    collided_ = false;
    collidedEntities_.clear();
}