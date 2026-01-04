#pragma once

#include "windows_base/include/interfaces/context.h"
#include "windows_base/include/interfaces/facade.h"

namespace wb
{
    class ISharedContext : public IContext
    {
    public:
        virtual ~ISharedContext() = default;
    };

    class ISharedFacade : public IFacade
    {
    public:
        virtual ~ISharedFacade() = default;
    };

} // namespace wb