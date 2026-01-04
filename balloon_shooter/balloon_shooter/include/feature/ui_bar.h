#pragma once
#include "windows_base/windows_base.h"

#include "balloon_shooter/include/feature/interfaces/ui_bar.h"

namespace balloon_shooter
{
    constexpr float DEFAULT_UI_BAR_MAX_VALUE = 100.0f; // デフォルトの最大値
    constexpr float DEFAULT_UI_BAR_MIN_VALUE = 0.0f;   // デフォルトの最小値

    class UIBarComponent : public IUIBarComponent
    {
    private:
        float maxValue_ = DEFAULT_UI_BAR_MAX_VALUE;
        float currentValue_ = 50.0f;
        float minValue_ = DEFAULT_UI_BAR_MIN_VALUE;

        std::unique_ptr<wb::IOptionalValue> frameEntityId_ = nullptr;
        DirectX::XMFLOAT4 frameColor_ = {1.0f, 1.0f, 1.0f, 1.0f}; // 白色

        std::unique_ptr<wb::IOptionalValue> barEntityId_ = nullptr;
        DirectX::XMFLOAT4 barColor_ = {0.0f, 1.0f, 0.0f, 1.0f}; // 緑色

    public:
        UIBarComponent() = default;
        virtual ~UIBarComponent() = default;

        /***************************************************************************************************************
         * IUIBarComponent implementation
        /**************************************************************************************************************/

        virtual void SetMaxValue(float maxValue) override { maxValue_ = maxValue; }
        virtual float GetMaxValue() const override { return maxValue_; }

        virtual void SetCurrentValue(float currentValue) override { currentValue_ = currentValue; }
        virtual float GetCurrentValue() const override { return currentValue_; }

        virtual void SetMinValue(float minValue) override { minValue_ = minValue; }
        virtual float GetMinValue() const override { return minValue_; }

        virtual void SetFrameEntityID(std::unique_ptr<wb::IOptionalValue> frameEntityId) override;
        virtual wb::IOptionalValue &GetFrameEntityID() const override;
        virtual void SetFrameColor(const DirectX::XMFLOAT4 &color) override { frameColor_ = color; }
        virtual DirectX::XMFLOAT4 GetFrameColor() const override { return frameColor_; }

        virtual void SetBarEntityID(std::unique_ptr<wb::IOptionalValue> barEntityId) override;
        virtual wb::IOptionalValue &GetBarEntityID() const override;
        virtual void SetBarColor(const DirectX::XMFLOAT4 &color) override { barColor_ = color; }
        virtual DirectX::XMFLOAT4 GetBarColor() const override { return barColor_; }
    };

    void ShowUIBar
    (
        wb::IEntity *entity, wb::IComponentContainer &componentCont, wb::IEntityContainer &entityCont,
        bool isShow, size_t componentID);

} // namespace balloon_shooter