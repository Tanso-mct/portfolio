#pragma once
#include "windows_base/windows_base.h"

namespace example
{
    const size_t &ControllerComponentID();

    class IControllerComponent : public wb::IComponent
    {
    public:
        virtual ~IControllerComponent() = default;

        virtual float &GetSpeed() = 0;
        virtual float &GetSensitivity() = 0;

        virtual void SetBodyEntityID(std::unique_ptr<wb::IOptionalValue> bodyEntityId) = 0;
        virtual wb::IOptionalValue &GetBodyEntityID() const = 0;
        virtual float &CurrentBodyPitch() = 0;
        virtual float &CurrentBodyYaw() = 0;

        virtual void SetCameraEntityID(std::unique_ptr<wb::IOptionalValue> cameraEntityId) = 0;
        virtual wb::IOptionalValue &GetCameraEntityID() const = 0;
        virtual float &CurrentCameraPitch() = 0;
        virtual float &CurrentCameraYaw() = 0;
    };


    class ControllerComponent : public IControllerComponent
    {
    private:
        float speed_ = 1.0f;
        float sensitivity_ = 0.1f;

        std::unique_ptr<wb::IOptionalValue> bodyEntityId_ = nullptr;
        float currentBodyPitch_ = 0.0f;
        float currentBodyYaw_ = 0.0f;

        std::unique_ptr<wb::IOptionalValue> cameraEntityId_ = nullptr;
        float currentCameraPitch_ = 0.0f;
        float currentCameraYaw_ = 0.0f;

    public:
        ControllerComponent() = default;
        virtual ~ControllerComponent() = default;

        ControllerComponent(const ControllerComponent &) = delete;
        ControllerComponent &operator=(const ControllerComponent &) = delete;

        /***************************************************************************************************************
         * IComponent implementation
        /**************************************************************************************************************/

        virtual const size_t &GetID() const override;

        /***************************************************************************************************************
         * IControllerComponent implementation
        /**************************************************************************************************************/

        virtual float &GetSpeed() override { return speed_; }
        virtual float &GetSensitivity() override { return sensitivity_; }

        virtual void SetBodyEntityID(std::unique_ptr<wb::IOptionalValue> bodyEntityId) override;
        virtual wb::IOptionalValue &GetBodyEntityID() const override;
        virtual float &CurrentBodyPitch() override { return currentBodyPitch_; }
        virtual float &CurrentBodyYaw() override { return currentBodyYaw_; }

        virtual void SetCameraEntityID(std::unique_ptr<wb::IOptionalValue> cameraEntityId) override;
        virtual wb::IOptionalValue &GetCameraEntityID() const override;
        virtual float &CurrentCameraPitch() override { return currentCameraPitch_; }
        virtual float &CurrentCameraYaw() override { return currentCameraYaw_; }

    };
}