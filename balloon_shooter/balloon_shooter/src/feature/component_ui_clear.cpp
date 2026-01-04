#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/component_ui_clear.h"

const size_t &balloon_shooter::ClearUIBackgroundComponentID()
{
    static size_t id = wb::IDFactory::CreateComponentID();
    return id;
}

const size_t &balloon_shooter::ClearUIBackgroundComponent::GetID() const
{
    return balloon_shooter::ClearUIBackgroundComponentID();
}

namespace balloon_shooter
{
    WB_REGISTER_COMPONENT(ClearUIBackgroundComponentID(), ClearUIBackgroundComponent);

} // namespace balloon_shooter

const size_t &balloon_shooter::ClearUIPlayBtnComponentID()
{
    static size_t id = wb::IDFactory::CreateComponentID();
    return id;
}

const size_t &balloon_shooter::ClearUIPlayBtnComponent::GetID() const
{
    return balloon_shooter::ClearUIPlayBtnComponentID();
}

namespace balloon_shooter
{
    WB_REGISTER_COMPONENT(ClearUIPlayBtnComponentID(), ClearUIPlayBtnComponent);

} // namespace balloon_shooter

const size_t &balloon_shooter::ClearUIMenuBtnComponentID()
{
    static size_t id = wb::IDFactory::CreateComponentID();
    return id;
}

const size_t &balloon_shooter::ClearUIMenuBtnComponent::GetID() const
{
    return balloon_shooter::ClearUIMenuBtnComponentID();
}

namespace balloon_shooter
{
    WB_REGISTER_COMPONENT(ClearUIMenuBtnComponentID(), ClearUIMenuBtnComponent);

} // namespace balloon_shooter