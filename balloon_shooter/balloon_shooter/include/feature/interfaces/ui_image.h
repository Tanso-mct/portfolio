#pragma once
#include "windows_base/windows_base.h"

#include "balloon_shooter/include/feature/interfaces/ui_element.h"

#include <DirectXMath.h>

namespace balloon_shooter
{
    class IUIImageComponent : public IUIElement
    {
    public:
        virtual ~IUIImageComponent() = default;
    };

} // namespace balloon_shooter