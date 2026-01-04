#include "mono_physics/src/pch.h"
#include "mono_physics/include/component_box_collider.h"

mono_physics::BoxCollisionResult::BoxCollisionResult()
{
}

mono_physics::BoxCollisionResult::~BoxCollisionResult()
{
    collisionNormals_.clear();
}

void mono_physics::BoxCollisionResult::Clear()
{
    // Clear base class data
    collided_ = false;
    collidedEntities_.clear();

    // Clear box collision specific data
    collisionNormals_.clear();
}

mono_physics::ComponentBoxCollider::ComponentBoxCollider()
{
}

mono_physics::ComponentBoxCollider::~ComponentBoxCollider()
{
}

void mono_physics::ComponentBoxCollider::Setup(SetupParam &param)
{
    isTrigger_ = param.isTrigger;
    collisionResult_ = std::move(param.collisionResult);

    boundingBox_ = *param.box; // Update bounding box
    shape_ = std::move(param.box);
}

const mono_physics::ShapeBox &mono_physics::ComponentBoxCollider::GetBox() const
{
    return static_cast<const ShapeBox&>(*shape_);
}

void mono_physics::ComponentBoxCollider::SetBox(std::unique_ptr<ShapeBox> box)
{
    boundingBox_ = *box; // Update bounding box
    shape_ = std::move(box);
}

const mono_physics::BoxCollisionResult &mono_physics::ComponentBoxCollider::GetBoxCollisionResult() const
{
    return static_cast<const BoxCollisionResult&>(*collisionResult_);
}

void mono_physics::ComponentBoxCollider::SetBoxCollisionResult(std::unique_ptr<BoxCollisionResult> result)
{
    collisionResult_ = std::move(result);
}

MONO_PHYSICS_API riaecs::ComponentRegistrar
<mono_physics::ComponentBoxCollider, mono_physics::ComponentBoxColliderMaxCount> mono_physics::ComponentBoxColliderID;