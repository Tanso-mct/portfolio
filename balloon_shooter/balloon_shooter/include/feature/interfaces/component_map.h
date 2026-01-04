#pragma once
#include "windows_base/windows_base.h"

#include <DirectXMath.h>

namespace balloon_shooter
{
    class IMapComponent : public wb::IComponent
    {
    public:
        virtual ~IMapComponent() = default;
    };

    class IMapBottomComponent : public wb::IComponent
    {
    public:
        virtual ~IMapBottomComponent() = default;
    };

} // namespace balloon_shooter