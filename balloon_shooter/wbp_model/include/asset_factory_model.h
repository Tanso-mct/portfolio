#pragma once
#include "wbp_model/include/dll_config.h"
#include "windows_base/windows_base.h"

namespace wbp_model
{
    const WBP_MODEL_API size_t &ModelAssetFactoryID();

    class WBP_MODEL_API ModelAssetFactory : public wb::IAssetFactory
    {
    public:
        ModelAssetFactory() = default;
        ~ModelAssetFactory() override = default;

        std::unique_ptr<wb::IAsset> Create(wb::IFileData &fileData) const override;
    };

} // namespace wbp_model