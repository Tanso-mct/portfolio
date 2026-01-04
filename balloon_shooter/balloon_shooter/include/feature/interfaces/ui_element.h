#pragma once
#include "windows_base/windows_base.h"

namespace balloon_shooter
{
    class IUIElement : public wb::IComponent
    {
    public:
        virtual ~IUIElement() = default;
    };

} // namespace balloon_shooter