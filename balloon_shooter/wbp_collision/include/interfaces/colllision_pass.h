#pragma once
#include "windows_base/windows_base.h"

namespace wbp_collision
{
    class ICollisionPass
    {
    public:
        virtual ~ICollisionPass() = default;
        virtual void Execute(const wb::SystemArgument &args) = 0;
    };

} // namespace wbp_collision