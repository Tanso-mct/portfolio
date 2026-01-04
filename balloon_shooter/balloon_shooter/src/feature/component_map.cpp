#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/component_map.h"

const size_t &balloon_shooter::MapComponentID()
{
    static size_t id = wb::IDFactory::CreateComponentID();
    return id;
}

const size_t &balloon_shooter::MapComponent::GetID() const
{
    return MapComponentID();
}

namespace balloon_shooter
{
    WB_REGISTER_COMPONENT(MapComponentID(), MapComponent);

} // namespace balloon_shooter

const size_t &balloon_shooter::MapBottomComponentID()
{
    static size_t id = wb::IDFactory::CreateComponentID();
    return id;
}

const size_t &balloon_shooter::MapBottomComponent::GetID() const
{
    return MapBottomComponentID();
}

namespace balloon_shooter
{
    WB_REGISTER_COMPONENT(MapBottomComponentID(), MapBottomComponent);

} // namespace balloon_shooter