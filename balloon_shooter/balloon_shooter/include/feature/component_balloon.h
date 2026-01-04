#pragma once
#include "windows_base/windows_base.h"

#include "balloon_shooter/include/feature/interfaces/component_balloon.h"

namespace balloon_shooter
{
    const size_t &BalloonComponentID();

    class BalloonComponent : public IBalloonComponent
    {
    private:
        float movingSpeed_ = 8.0f;
        float moveLimit_ = 60.0f;
        float currentMoveValue_ = 0.0f;
        float moveSide_ = 1.0f;

        DirectX::XMFLOAT3 baseCenter_ = {0.0f, 0.0f, 0.0f};

    public:
        BalloonComponent() = default;
        virtual ~BalloonComponent() = default;

        BalloonComponent(const BalloonComponent &) = delete;
        BalloonComponent &operator=(const BalloonComponent &) = delete;

        /***************************************************************************************************************
         * IComponent implementation
        /**************************************************************************************************************/
        
        const size_t &GetID() const override;

        /***************************************************************************************************************
         * IBalloonComponent implementation
        /**************************************************************************************************************/

        void SetMovingSpeed(float speed) override { movingSpeed_ = speed; }
        float GetMovingSpeed() const override { return movingSpeed_; }

        void SetMoveLimit(float limit) override { moveLimit_ = limit; }
        float GetMoveLimit() const override { return moveLimit_; }

        void SetCurrentMoveValue(float value) override { currentMoveValue_ = value; }
        float GetCurrentMoveValue() const override { return currentMoveValue_; }

        void SetMoveSide(float side) override { moveSide_ = side; }
        float GetMoveSide() const override { return moveSide_; }

        void SetBaseCenter(const DirectX::XMFLOAT3 &center) override { baseCenter_ = center; }
        const DirectX::XMFLOAT3 &GetBaseCenter() const override { return baseCenter_; }

    };

} // namespace balloon_shooter