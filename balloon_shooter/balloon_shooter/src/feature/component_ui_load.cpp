#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/component_ui_load.h"

const size_t &balloon_shooter::LoadUIBackgroundComponentID()
{
    static size_t id = wb::IDFactory::CreateComponentID();
    return id;
}

const size_t &balloon_shooter::LoadUIBackgroundComponent::GetID() const
{
    return balloon_shooter::LoadUIBackgroundComponentID();
}

namespace balloon_shooter
{
    WB_REGISTER_COMPONENT(LoadUIBackgroundComponentID(), LoadUIBackgroundComponent);

} // namespace balloon_shooter