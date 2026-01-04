#pragma once
#include "wbp_locator/include/dll_config.h"
#include "windows_base/windows_base.h"

#include "wbp_locator/include/interfaces/component_locator.h"

namespace wbp_locator
{
    const WBP_LOCATOR_API size_t &LocatorComponentID();

    constexpr size_t DEFAULT_LOCATE_TARGET_INDEX = 0;

    class WBP_LOCATOR_API LocatorComponent : public ILocatorComponent
    {
    private:
        size_t locatorAssetID_ = 0;
        bool isAssetIDSet_ = false;

        size_t locateTargetIndex_ = DEFAULT_LOCATE_TARGET_INDEX;
        bool isLocated_ = false;

    public:
        LocatorComponent() = default;
        ~LocatorComponent() override = default;

        /***************************************************************************************************************
         * IComponent implementation
        /**************************************************************************************************************/

        const size_t &GetID() const override;

        /***************************************************************************************************************
         * ILocatorComponent implementation
        /**************************************************************************************************************/

        void SetLocatorAssetID(const size_t &locatorAssetID) override;
        const size_t &GetLocatorAssetID() const override;

        void SetLocateTargetIndex(const size_t &index) override { locateTargetIndex_ = index; }
        const size_t &GetLocateTargetIndex() const override { return locateTargetIndex_; }

        bool IsLocated() const override { return isLocated_; }
        void SetLocated(bool located) override { isLocated_ = located; }
    };

} // namespace wbp_locator