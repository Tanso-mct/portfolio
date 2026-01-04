#pragma once

#include "windows_base/include/interfaces/context.h"

#include <memory>

namespace wb
{
    class IFacade
    {
    public:
        virtual ~IFacade() = default;
        virtual void SetContext(std::unique_ptr<IContext> context) = 0;
        virtual bool CheckIsReady() const = 0;
    };

} // namespace wb