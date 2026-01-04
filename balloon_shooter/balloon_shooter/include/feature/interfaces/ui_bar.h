#pragma once
#include "windows_base/windows_base.h"

#include "balloon_shooter/include/feature/interfaces/ui_element.h"

#include <DirectXMath.h>

namespace balloon_shooter
{
    constexpr float UI_BAR_FRAME_BAR_OFFSET = 10.0f; // フレームとバーの間のオフセット

    class IUIBarComponent : public IUIElement
    {
    public:
        virtual ~IUIBarComponent() = default;

        virtual void SetMaxValue(float maxValue) = 0;
        virtual float GetMaxValue() const = 0;

        virtual void SetCurrentValue(float currentValue) = 0;
        virtual float GetCurrentValue() const = 0;
        
        virtual void SetMinValue(float minValue) = 0;
        virtual float GetMinValue() const = 0;

        virtual void SetFrameEntityID(std::unique_ptr<wb::IOptionalValue> frameEntityId) = 0;
        virtual wb::IOptionalValue &GetFrameEntityID() const = 0;
        virtual void SetFrameColor(const DirectX::XMFLOAT4 &color) = 0;
        virtual DirectX::XMFLOAT4 GetFrameColor() const = 0;

        virtual void SetBarEntityID(std::unique_ptr<wb::IOptionalValue> barEntityId) = 0;
        virtual wb::IOptionalValue &GetBarEntityID() const = 0;
        virtual void SetBarColor(const DirectX::XMFLOAT4 &color) = 0;
        virtual DirectX::XMFLOAT4 GetBarColor() const = 0;
    };

} // namespace balloon_shooter