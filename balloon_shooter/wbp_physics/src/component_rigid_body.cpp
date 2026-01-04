#include "wbp_physics/src/pch.h"
#include "wbp_physics/include/component_rigid_body.h"

const WBP_PHYSICS_API size_t &wbp_physics::RigidBodyComponentID()
{
    static size_t id = wb::IDFactory::CreateComponentID();
    return id;
}

const size_t &wbp_physics::RigidBodyComponent::GetID() const
{
    return wbp_physics::RigidBodyComponentID();
}

namespace wbp_physics
{
    WB_REGISTER_COMPONENT(RigidBodyComponentID(), RigidBodyComponent);

} // namespace wbp_physics