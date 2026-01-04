#pragma once
#include "windows_base/windows_base.h"

namespace balloon_shooter
{
    const size_t &MapModelAssetID();
    const size_t &MapColliderShapeAssetID();
    const size_t &MapTextureAssetID();

    const size_t &MapBottomColliderShapeAssetID();

    class MapPrefab : public wb::IPrefab
    {
    public:
        MapPrefab() = default;
        ~MapPrefab() override = default;

        std::unique_ptr<wb::IOptionalValue> Create
        (
            wb::IAssetContainer &assetCont,
            wb::IEntityContainer &entityCont,
            wb::IComponentContainer &componentCont,
            wb::IEntityIDView &entityIDView
        ) const override;

        std::vector<size_t> GetNeedAssetIDs() const override;

    };

} // namespace balloon_shooter