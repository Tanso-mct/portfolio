#pragma once
#include "windows_base/windows_base.h"

#include <DirectXMath.h>

namespace balloon_shooter
{
    class IPlayerComponent : public wb::IComponent
    {
    public:
        virtual ~IPlayerComponent() = default;

        virtual float &GetSpeed() = 0;
        virtual float &GetDashSpeed() = 0;
        virtual float &GetSensitivity() = 0;

        virtual float &GetJumpPower() = 0;
        virtual float &GetGravity() = 0;
        virtual UINT &GetJumpFrameCount() = 0;

        virtual float &GetVerticalVelocity() = 0;
        virtual bool &IsGrounded() = 0;

        virtual void SetBodyEntityID(std::unique_ptr<wb::IOptionalValue> bodyEntityId) = 0;
        virtual wb::IOptionalValue &GetBodyEntityID() const = 0;
        virtual float &CurrentCameraPitch() = 0;
        virtual float &CurrentCameraYaw() = 0;

        virtual void SetCameraEntityID(std::unique_ptr<wb::IOptionalValue> cameraEntityId) = 0;
        virtual wb::IOptionalValue &GetCameraEntityID() const = 0;
        virtual float &CurrentBodyYaw() = 0;

        virtual void SetGunEntityID(std::unique_ptr<wb::IOptionalValue> gunEntityId) = 0;
        virtual wb::IOptionalValue &GetGunEntityID() const = 0;

        virtual void SetRecoilValue(float value) = 0;
        virtual float GetRecoilValue() const = 0;

        virtual void SetAnimStarted(bool flag) = 0;
        virtual bool IsAnimStarted() const = 0;

        virtual void SetCurrentGunPitch(float value) = 0;
        virtual float GetCurrentGunPitch() const = 0;

        virtual void SetRecoilReached(bool flag) = 0;
        virtual bool IsRecoilReached() const = 0;

        virtual void SetRecoilRiseSpeed(float speed) = 0;
        virtual float GetRecoilRiseSpeed() const = 0;

        virtual void SetMuzzleClimbSpeed(float speed) = 0;
        virtual float GetMuzzleClimbSpeed() const = 0;

        virtual const DirectX::XMFLOAT3 &GetCameraOffset() = 0;
        virtual void SetCameraOffset(const DirectX::XMFLOAT3 &offset) = 0;

        virtual void SetRayEntityID(std::unique_ptr<wb::IOptionalValue> rayEntityId) = 0;
        virtual wb::IOptionalValue &GetRayEntityID() const = 0;

        virtual void SetRayCasted(bool casted) = 0;
        virtual bool IsRayCasted() const = 0;
        
    };

} // namespace balloon_shooter