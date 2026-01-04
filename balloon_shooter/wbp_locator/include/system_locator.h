#pragma once
#include "wbp_locator/include/dll_config.h"
#include "windows_base/windows_base.h"

namespace wbp_locator
{
    const WBP_LOCATOR_API size_t &LocatorSystemID();

    class WBP_LOCATOR_API LocatorSystem : public wb::ISystem
    {
    public:
        LocatorSystem() = default;
        ~LocatorSystem() override = default;

        const size_t &GetID() const override;
        void Initialize(wb::IAssetContainer &assetContainer) override;
        void Update(const wb::SystemArgument &args) override;
    };

} // namespace wbp_locator