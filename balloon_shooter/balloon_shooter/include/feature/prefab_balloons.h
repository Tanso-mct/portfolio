#pragma once
#include "windows_base/windows_base.h"

namespace balloon_shooter
{
    const size_t &BalloonModelAssetID();
    const size_t &BalloonColliderShapeAssetID();
    const size_t &BalloonLocatorAssetID();

    const size_t &BalloonRedTextureAssetID();
    const size_t &BalloonBlueTextureAssetID();
    const size_t &BalloonGreenTextureAssetID();
    const size_t &BalloonYellowTextureAssetID();
    const size_t &BalloonPurpleTextureAssetID();
    const size_t &BalloonPinkTextureAssetID();

    class BalloonsPrefab : public wb::IPrefab
    {
    public:
        BalloonsPrefab() = default;
        ~BalloonsPrefab() override = default;

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