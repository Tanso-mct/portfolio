#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/component_ui_title.h"

const size_t &balloon_shooter::TitleUIBackgroundComponentID()
{
    static size_t id = wb::IDFactory::CreateComponentID();
    return id;
}

const size_t &balloon_shooter::TitleUIBackgroundComponent::GetID() const
{
    return balloon_shooter::TitleUIBackgroundComponentID();
}

namespace balloon_shooter
{
    WB_REGISTER_COMPONENT(TitleUIBackgroundComponentID(), TitleUIBackgroundComponent);

} // namespace balloon_shooter

const size_t &balloon_shooter::TitleUIPlayBtnComponentID()
{
    static size_t id = wb::IDFactory::CreateComponentID();
    return id;
}

const size_t &balloon_shooter::TitleUIPlayBtnComponent::GetID() const
{
    return balloon_shooter::TitleUIPlayBtnComponentID();
}

namespace balloon_shooter
{
    WB_REGISTER_COMPONENT(TitleUIPlayBtnComponentID(), TitleUIPlayBtnComponent);

} // namespace balloon_shooter

const size_t &balloon_shooter::TitleUIExitBtnComponentID()
{
    static size_t id = wb::IDFactory::CreateComponentID();
    return id;
}

const size_t &balloon_shooter::TitleUIExitBtnComponent::GetID() const
{
    return balloon_shooter::TitleUIExitBtnComponentID();
}

namespace balloon_shooter
{
    WB_REGISTER_COMPONENT(TitleUIExitBtnComponentID(), TitleUIExitBtnComponent);

} // namespace balloon_shooter