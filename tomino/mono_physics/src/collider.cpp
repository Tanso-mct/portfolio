#include "mono_physics/src/pch.h"
#include "mono_physics/include/collider.h"

mono_physics::Collider::Collider()
{
}

mono_physics::Collider::~Collider()
{
    shape_ = nullptr;
    isTrigger_ = false;
    collisionEnabled_ = true;

    collisionResult_ = nullptr;
    
    collidableComponentIDs_.clear();
}