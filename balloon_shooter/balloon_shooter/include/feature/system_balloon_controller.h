#pragma once
#include "windows_base/windows_base.h"

namespace balloon_shooter
{
    const size_t &BalloonControllerSystemID();

    class BalloonControllerSystem : public wb::ISystem
    {
    private:
        bool isStarted = false;

    public:
        BalloonControllerSystem() = default;
        ~BalloonControllerSystem() override = default;

        /***************************************************************************************************************
         * ISystem implementation
        /**************************************************************************************************************/

        const size_t &GetID() const override;
        void Initialize(wb::IAssetContainer &assetContainer) override;
        void Update(const wb::SystemArgument &args) override;

    };

} // namespace balloon_shooter