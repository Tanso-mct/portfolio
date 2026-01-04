#pragma once
#include "wbp_locator/include/dll_config.h"
#include "windows_base/windows_base.h"

namespace wbp_locator
{
    const WBP_LOCATOR_API size_t &LocatorAssetFactoryID();

    class WBP_LOCATOR_API LocatorAssetFactory : public wb::IAssetFactory
    {
    public:
        LocatorAssetFactory() = default;
        ~LocatorAssetFactory() override = default;

        std::unique_ptr<wb::IAsset> Create(wb::IFileData &fileData) const override;
    };

} // namespace wbp_locator