#include "wbp_collision/src/pch.h"
#include "wbp_collision/include/component_ray_collider.h"

const WBP_COLLISION_API size_t &wbp_collision::RayColliderComponentID()
{
    static size_t id = wb::IDFactory::CreateComponentID();
    return id;
}

const size_t &wbp_collision::RayColliderComponent::GetID() const
{
    return RayColliderComponentID();
}

void wbp_collision::RayColliderComponent::AddTargetComponentID(const size_t &id)
{
    if (std::find(targetComponentIDs_.begin(), targetComponentIDs_.end(), id) == targetComponentIDs_.end())
    {
        targetComponentIDs_.push_back(id);
    }
}

void wbp_collision::RayColliderComponent::RemoveTargetComponentID(const size_t &id)
{
    size_t eraseIndex = 0;
    bool found = false;

    for (size_t i = 0; i < targetComponentIDs_.size(); ++i)
    {
        if (targetComponentIDs_[i] == id)
        {
            eraseIndex = i;
            found = true;
            break;
        }
    }

    if (found)
    {
        targetComponentIDs_.erase(targetComponentIDs_.begin() + eraseIndex);
    }
    else
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "Target component ID not found in the list.",
            }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WBP_COLLISION", err);
        wb::ThrowRuntimeError(err);
    }
}

namespace wbp_collision
{
    WB_REGISTER_COMPONENT(RayColliderComponentID(), RayColliderComponent);

} // namespace wbp_collision