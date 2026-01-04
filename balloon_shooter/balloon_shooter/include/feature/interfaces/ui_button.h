#pragma once
#include "windows_base/windows_base.h"

#include "balloon_shooter/include/feature/interfaces/ui_element.h"

#include <DirectXMath.h>

namespace balloon_shooter
{
    class IButtonEvent
    {
    public:
        virtual ~IButtonEvent() = default;

        virtual void OnClick(const wb::SystemArgument &args) = 0;
        virtual void OnHover(const wb::SystemArgument &args) = 0;
        virtual void OnUnHover(const wb::SystemArgument &args) = 0;
    };

    class IUIButtonComponent : public IUIElement
    {
    public:
        virtual ~IUIButtonComponent() override = default;

        virtual void SetEvent(std::unique_ptr<IButtonEvent> event) = 0;
        virtual IButtonEvent &GetEvent()  = 0;

        virtual void SetIsHovering(bool isHovering) = 0;
        virtual bool IsHovering() const = 0;

    };

} // namespace balloon_shooter