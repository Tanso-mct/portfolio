#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/component_balloon.h"

const size_t &balloon_shooter::BalloonComponentID()
{
    static size_t id = wb::IDFactory::CreateComponentID();
    return id;
}

const size_t &balloon_shooter::BalloonComponent::GetID() const
{
    return BalloonComponentID();
}

namespace balloon_shooter
{
    WB_REGISTER_COMPONENT(BalloonComponentID(), BalloonComponent);

} // namespace balloon_shooter