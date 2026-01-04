#pragma once
#include "windows_base/windows_base.h"

#include "balloon_shooter/include/feature/interfaces/component_player.h"

namespace balloon_shooter
{
    const size_t &PlayerComponentID();

    constexpr float PLAYER_SPEED_DEFAULT = 1.0f; // 移動速度の初期値
    constexpr float PLAYER_DASH_SPEED_DEFAULT = 2.0f; // ダッシュ速度の初期値
    constexpr float PLAYER_SENSITIVITY_DEFAULT = 0.1f; // マウス感度の初期値

    constexpr float PLAYER_JUMP_POWER_DEFAULT = 10.0f; // ジャンプ力の初期値
    constexpr float PLAYER_GRAVITY_DEFAULT = 9.81f; // 重力の初期値

    class PlayerComponent : public IPlayerComponent
    {
    private:
        float speed_ = PLAYER_SPEED_DEFAULT;
        float dashSpeed_ = PLAYER_DASH_SPEED_DEFAULT;
        float sensitivity_ = PLAYER_SENSITIVITY_DEFAULT;

        float jumpPower_ = PLAYER_JUMP_POWER_DEFAULT;
        float gravity_ = PLAYER_GRAVITY_DEFAULT;
        UINT jumpFrameCount_ = 0;

        float verticalVelocity_ = 0.0f;
        bool isGrounded_ = false;

        std::unique_ptr<wb::IOptionalValue> cameraEntityId_ = nullptr;
        float currentCameraPitch_ = 0.0f;
        float currentCameraYaw_ = 0.0f;

        std::unique_ptr<wb::IOptionalValue> bodyEntityId_ = nullptr;
        float currentBodyYaw_ = 0.0f;

        std::unique_ptr<wb::IOptionalValue> gunEntityId_ = nullptr;
        float recoilValue_ = 0.0f; // リコイル値
        bool animStarted_ = false; // アニメーションが開始されたかどうか
        float currentGunPitch_ = 0.0f; // 現在の銃のピッチ角度
        bool recoilReached_ = false; // リコイルが到達したかどうか

        float recoilRiseSpeed_ = 0.0f; // リコイルの上昇速度
        float muzzleClimbSpeed_ = 0.0f; // マズルのクライム速度

        DirectX::XMFLOAT3 cameraOffset_ = {0.0f, 0.0f, 0.0f}; // カメラのオフセット

        std::unique_ptr<wb::IOptionalValue> rayEntityId_ = nullptr;
        bool rayCasted_ = false; // レイがキャストされたかどうか

    public:
        PlayerComponent() = default;
        virtual ~PlayerComponent() = default;

        PlayerComponent(const PlayerComponent &) = delete;
        PlayerComponent &operator=(const PlayerComponent &) = delete;

        /***************************************************************************************************************
         * IComponent implementation
        /**************************************************************************************************************/

        const size_t &GetID() const override;

        /***************************************************************************************************************
         * IPlayerComponent implementation
        /**************************************************************************************************************/

        float &GetSpeed() override { return speed_; }
        float &GetDashSpeed() override { return dashSpeed_; }
        float &GetSensitivity() override { return sensitivity_; }

        float &GetJumpPower() override { return jumpPower_; }
        float &GetGravity() override { return gravity_; }
        UINT &GetJumpFrameCount() override { return jumpFrameCount_; }

        float &GetVerticalVelocity() override { return verticalVelocity_; }
        bool &IsGrounded() override { return isGrounded_; }

        void SetBodyEntityID(std::unique_ptr<wb::IOptionalValue> bodyEntityId) override;
        wb::IOptionalValue &GetBodyEntityID() const override;
        float &CurrentCameraPitch() override { return currentCameraPitch_; }
        float &CurrentCameraYaw() override { return currentCameraYaw_; }

        void SetCameraEntityID(std::unique_ptr<wb::IOptionalValue> cameraEntityId) override;
        wb::IOptionalValue &GetCameraEntityID() const override;
        float &CurrentBodyYaw() override { return currentBodyYaw_; }

        void SetGunEntityID(std::unique_ptr<wb::IOptionalValue> gunEntityId) override;
        wb::IOptionalValue &GetGunEntityID() const override;

        void SetRecoilValue(float value) override { recoilValue_ = value; }
        float GetRecoilValue() const override { return recoilValue_; }

        void SetRecoilReached(bool flag) override { recoilReached_ = flag; }
        bool IsRecoilReached() const override { return recoilReached_; }

        void SetAnimStarted(bool flag) override { animStarted_ = flag; }
        bool IsAnimStarted() const override { return animStarted_; }

        void SetCurrentGunPitch(float value) override { currentGunPitch_ = value; }
        float GetCurrentGunPitch() const override { return currentGunPitch_; }

        void SetRecoilRiseSpeed(float speed) override { recoilRiseSpeed_ = speed; }
        float GetRecoilRiseSpeed() const override { return recoilRiseSpeed_; }

        void SetMuzzleClimbSpeed(float speed) override { muzzleClimbSpeed_ = speed; }
        float GetMuzzleClimbSpeed() const override { return muzzleClimbSpeed_; }

        const DirectX::XMFLOAT3 &GetCameraOffset() override { return cameraOffset_; }
        void SetCameraOffset(const DirectX::XMFLOAT3 &offset) override { cameraOffset_ = offset; }

        void SetRayEntityID(std::unique_ptr<wb::IOptionalValue> rayEntityId) override;
        wb::IOptionalValue &GetRayEntityID() const override;

        void SetRayCasted(bool casted) override { rayCasted_ = casted; }
        bool IsRayCasted() const override { return rayCasted_; }
    };

} // namespace balloon_shooter