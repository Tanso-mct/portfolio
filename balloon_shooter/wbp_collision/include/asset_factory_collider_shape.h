#pragma once
#include "wbp_collision/include/dll_config.h"
#include "windows_base/windows_base.h"

namespace wbp_collision
{
    const WBP_COLLISION_API size_t &ColliderShapeAssetFactoryID();

    class WBP_COLLISION_API ColliderShapeAssetFactory : public wb::IAssetFactory
    {
    public:
        ColliderShapeAssetFactory() = default;
        ~ColliderShapeAssetFactory() override = default;

        std::unique_ptr<wb::IAsset> Create(wb::IFileData &fileData) const override;
    };

} // namespace wbp_collision