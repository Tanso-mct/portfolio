#pragma once
#include "windows_base/windows_base.h"

#include "balloon_shooter/include/feature/interfaces/ui_button.h"

namespace balloon_shooter
{
    const size_t &ClearUIBackgroundTextureAssetID();
    const size_t &ClearUIPlayBtnTextureAssetID();
    const size_t &ClearUIMenuBtnTextureAssetID();

    class ClearUIPlayBtnEvent : public IButtonEvent
    {
    private:
        float hoverScaleAmount_ = 1.1f;

    public:
        ClearUIPlayBtnEvent() = default;
        ~ClearUIPlayBtnEvent() override = default;

        void OnClick(const wb::SystemArgument &args) override;
        void OnHover(const wb::SystemArgument &args) override;
        void OnUnHover(const wb::SystemArgument &args) override;
    };

    class ClearUIMenuBtnEvent : public IButtonEvent
    {
    private:
        float hoverScaleAmount_ = 1.1f;

    public:
        ClearUIMenuBtnEvent() = default;
        ~ClearUIMenuBtnEvent() override = default;

        void OnClick(const wb::SystemArgument &args) override;
        void OnHover(const wb::SystemArgument &args) override;
        void OnUnHover(const wb::SystemArgument &args) override;
    };

    class ClearUIPrefab : public wb::IPrefab
    {
    public:
        ClearUIPrefab() = default;
        ~ClearUIPrefab() override = default;

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