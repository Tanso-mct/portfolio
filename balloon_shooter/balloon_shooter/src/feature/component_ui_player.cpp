#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/component_ui_player.h"

const size_t &balloon_shooter::PlayerUIComponentID()
{
    static size_t id = wb::IDFactory::CreateComponentID();
    return id;
}

const size_t &balloon_shooter::PlayerUIComponent::GetID() const
{
    return balloon_shooter::PlayerUIComponentID();
}

namespace balloon_shooter
{
    WB_REGISTER_COMPONENT(PlayerUIComponentID(), PlayerUIComponent);

} // namespace balloon_shooter

const size_t &balloon_shooter::PlayerUIBalloonLeftComponentID()
{
    static size_t id = wb::IDFactory::CreateComponentID();
    return id;
}

const size_t &balloon_shooter::PlayerUIBalloonLeftComponent::GetID() const
{
    return balloon_shooter::PlayerUIBalloonLeftComponentID();
}

namespace balloon_shooter
{
    WB_REGISTER_COMPONENT(PlayerUIBalloonLeftComponentID(), PlayerUIBalloonLeftComponent);

} // namespace balloon_shooter