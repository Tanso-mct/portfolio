#pragma once
#include "windows_base/windows_base.h"

namespace example
{
    const size_t &ControllerSystemID();

    class ControllerSystem : public wb::ISystem
    {
    public:
        ControllerSystem() = default;
        virtual ~ControllerSystem() = default;

        ControllerSystem(const ControllerSystem &) = delete;
        ControllerSystem &operator=(const ControllerSystem &) = delete;

        /***************************************************************************************************************
         * ISystem implementation
        /**************************************************************************************************************/

        virtual const size_t &GetID() const override;
        virtual void Initialize(wb::IAssetContainer &assetContainer) override;
        virtual void Update(const wb::SystemArgument &args) override;
    };

} // namespace example