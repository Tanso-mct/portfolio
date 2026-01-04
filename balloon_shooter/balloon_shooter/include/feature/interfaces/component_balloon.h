#pragma once
#include "windows_base/windows_base.h"

#include <DirectXMath.h>

namespace balloon_shooter
{
    class IBalloonComponent : public wb::IComponent
    {
    public:
        virtual ~IBalloonComponent() = default;

        virtual void SetMovingSpeed(float speed) = 0;
        virtual float GetMovingSpeed() const = 0;

        virtual void SetMoveLimit(float limit) = 0;
        virtual float GetMoveLimit() const = 0;

        virtual void SetCurrentMoveValue(float value) = 0;
        virtual float GetCurrentMoveValue() const = 0;

        virtual void SetMoveSide(float side) = 0;
        virtual float GetMoveSide() const = 0;

        virtual void SetBaseCenter(const DirectX::XMFLOAT3 &center) = 0;
        virtual const DirectX::XMFLOAT3 &GetBaseCenter() const = 0;

    };

} // namespace balloon_shooter