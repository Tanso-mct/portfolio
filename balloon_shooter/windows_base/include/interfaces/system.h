#pragma once

#include "windows_base/include/interfaces/container.h"

namespace wb
{
    class SystemArgument;

    class ISystem
    {
    public:
        virtual ~ISystem() = default;
        virtual const size_t &GetID() const = 0;

        virtual void Initialize(IAssetContainer &assetContainer) = 0;
        virtual void Update(const SystemArgument &args) = 0;
    };

    class ISystemScheduler
    {
    public:
        virtual ~ISystemScheduler() = default;
        virtual void Execute(ISystemContainer &systemCont, SystemArgument &args) = 0;
    };

} // namespace wb