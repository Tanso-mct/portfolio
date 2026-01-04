#pragma once
#include "windows_base/windows_base.h"

namespace balloon_shooter
{
    const size_t &PlayerUITextureAssetID();

    class PlayerUIPrefab : public wb::IPrefab
    {
    public:
        PlayerUIPrefab() = default;
        ~PlayerUIPrefab() override = default;

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