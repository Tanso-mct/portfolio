#pragma once

#include <DirectXMath.h>

using namespace DirectX;

#include "ecs/include/component.h"
#include "render_graph/include/resource_handle.h"
#include "mono_service/include/service.h"
#include "mono_graphics_extension/include/dll_config.h"

namespace mono_graphics_extension
{

// The handle class for the camera component
class MONO_GRAPHICS_EXT_DLL CameraComponentHandle : public ecs::ComponentHandle<CameraComponentHandle> {};

constexpr float DEFAULT_FOV_Y = XMConvertToRadians(45.0f);
constexpr float DEFAULT_ASPECT_RATIO = 16.0f / 9.0f;
constexpr float DEFAULT_NEAR_Z = 0.1f;
constexpr float DEFAULT_FAR_Z = 1000.0f;

// The camera component class
class MONO_GRAPHICS_EXT_DLL CameraComponent :
    public ecs::Component
{
public:
    CameraComponent(std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy);
    virtual ~CameraComponent() override;

    class SetupParam : //REFLECTABLE_CLASS_BEGIN//
        public ecs::Component::SetupParam
    {
    public:
        SetupParam() : 
            fov_y(DEFAULT_FOV_Y),
            aspect_ratio(DEFAULT_ASPECT_RATIO), 
            near_z(DEFAULT_NEAR_Z), 
            far_z(DEFAULT_FAR_Z)
        {
        }

        virtual ~SetupParam() override = default;

        // Field of view in degrees
        float fov_y; //REFLECTED//

        // Aspect ratio (width / height)
        float aspect_ratio; //REFLECTED//

        // Near clipping plane distance
        float near_z; //REFLECTED//

        // Far clipping plane distance
        float far_z; //REFLECTED//

    }; //REFLECTABLE_CLASS_END//

    virtual bool Setup(ecs::Component::SetupParam& param) override;
    virtual bool Apply(const ecs::Component::SetupParam& param) override;
    virtual ecs::ComponentID GetID() const override;

    // Get field of view in radians
    float GetFovY() const { return fov_y_; }

    // Set field of view in radians
    void SetFovY(float fov_y) { fov_y_ = fov_y; }

    // Get aspect ratio (width / height)
    float GetAspectRatio() const { return aspect_ratio_; }

    // Set aspect ratio (width / height)
    void SetAspectRatio(float aspect_ratio) { aspect_ratio_ = aspect_ratio; }

    // Get near clipping plane distance
    float GetNearZ() const { return near_z_; }

    // Set near clipping plane distance
    void SetNearZ(float near_z) { near_z_ = near_z; }

    // Get far clipping plane distance
    float GetFarZ() const { return far_z_; }

    // Set far clipping plane distance
    void SetFarZ(float far_z) { far_z_ = far_z; }

    // Get the view matrix buffer handle
    const render_graph::ResourceHandle* GetViewProjMatrixBufferHandle() const { 
        return &view_proj_matrix_buffer_handle_; }

    // Get the inverse view-projection matrix buffer handle
    const render_graph::ResourceHandle* GetInvViewProjMatrixBufferHandle() const { 
        return &inv_view_proj_matrix_buffer_handle_; }

private:
    // The graphics service proxy
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy_ = nullptr;

    // Field of view in radians
    float fov_y_ = DEFAULT_FOV_Y;

    // Aspect ratio (width / height)
    float aspect_ratio_ = DEFAULT_ASPECT_RATIO;

    // Near clipping plane distance
    float near_z_ = DEFAULT_NEAR_Z;

    // Far clipping plane distance
    float far_z_ = DEFAULT_FAR_Z;

    // The view matrix buffer handle
    render_graph::ResourceHandle view_proj_matrix_buffer_handle_ = render_graph::ResourceHandle();

    // The inverse view-projection matrix buffer handle
    render_graph::ResourceHandle inv_view_proj_matrix_buffer_handle_ = render_graph::ResourceHandle();
};

} // namespace mono_graphics_extension