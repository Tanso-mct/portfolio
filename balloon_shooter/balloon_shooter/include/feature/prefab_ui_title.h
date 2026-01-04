#pragma once
#include "windows_base/windows_base.h"

#include "balloon_shooter/include/feature/interfaces/ui_button.h"

namespace balloon_shooter
{
    const size_t &TitleUIBackgroundTextureAssetID();
    const size_t &TitleUIPlayBtnTextureAssetID();
    const size_t &TitleUIExitBtnTextureAssetID();

    class TitleUIPlayBtnEvent : public IButtonEvent
    {
    private:
        float hoverScaleAmount_ = 1.1f;

    public:
        TitleUIPlayBtnEvent() = default;
        ~TitleUIPlayBtnEvent() override = default;

        void OnClick(const wb::SystemArgument &args) override;
        void OnHover(const wb::SystemArgument &args) override;
        void OnUnHover(const wb::SystemArgument &args) override;
    };

    class TitleUIExitBtnEvent : public IButtonEvent
    {
    private:
        float hoverScaleAmount_ = 1.1f;

    public:
        TitleUIExitBtnEvent() = default;
        ~TitleUIExitBtnEvent() override = default;

        void OnClick(const wb::SystemArgument &args) override;
        void OnHover(const wb::SystemArgument &args) override;
        void OnUnHover(const wb::SystemArgument &args) override;
    };

    class TitleUIPrefab : public wb::IPrefab
    {
    public:
        TitleUIPrefab() = default;
        ~TitleUIPrefab() override = default;

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