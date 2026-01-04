#pragma once
#include "wbp_render/include/dll_config.h"
#include "windows_base/windows_base.h"

#include "wbp_render/include/interfaces/component_camera.h"

namespace wbp_render
{
    const WBP_RENDER_API size_t &CameraComponentID();

    constexpr float DEFAULT_CAMERA_FOV = 45.0f;
    constexpr float DEFAULT_CAMERA_NEAR_Z = 0.1f;
    constexpr float DEFAULT_CAMERA_FAR_Z = 1000.0f;

    class WBP_RENDER_API CameraComponent : public ICameraComponent
    {
    private:
        float fieldOfView_ = DEFAULT_CAMERA_FOV;
        float nearZ_ = DEFAULT_CAMERA_NEAR_Z;
        float farZ_ = DEFAULT_CAMERA_FAR_Z;

    public:
        CameraComponent() = default;
        ~CameraComponent() override = default;

        /***************************************************************************************************************
         * ICameraComponent implementation
        /**************************************************************************************************************/

        const size_t &GetID() const override;

        /***************************************************************************************************************
         * ICameraComponent implementation
        /**************************************************************************************************************/

        const float &GetFieldOfView() const override { return fieldOfView_; }
        void SetFieldOfView(float fov) override { fieldOfView_ = fov; }

        const float &GetNearZ() const override { return nearZ_; }
        void SetNearZ(float nearZ) override { nearZ_ = nearZ; }

        const float &GetFarZ() const override { return farZ_; }
        void SetFarZ(float farZ) override { farZ_ = farZ; }
    };

} // namespace wbp_render