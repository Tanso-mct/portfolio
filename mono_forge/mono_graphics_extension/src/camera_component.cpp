#include "mono_graphics_extension/src/pch.h"
#include "mono_graphics_extension/include/camera_component.h"

#include "mono_graphics_service/include/graphics_command_list.h"

namespace mono_graphics_extension
{

CameraComponent::CameraComponent(std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy) :
    graphics_service_proxy_(std::move(graphics_service_proxy))
{
}

CameraComponent::~CameraComponent()
{
    // Create graphics service command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = graphics_service_proxy_->CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(command_list.get());
    assert(graphics_command_list != nullptr && "Failed to create GraphicsCommandList");

    // Destroy view matrix buffer
    graphics_command_list->DestroyResource(&view_proj_matrix_buffer_handle_);

    // Destroy inverse view-projection matrix buffer
    graphics_command_list->DestroyResource(&inv_view_proj_matrix_buffer_handle_);

    // Submit command list to graphics service
    graphics_service_proxy_->SubmitCommandList(std::move(command_list));
}

bool CameraComponent::Setup(ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    CameraComponent::SetupParam* camera_param
        = dynamic_cast<CameraComponent::SetupParam*>(&param);
    assert(camera_param != nullptr && "Invalid setup param type for CameraComponent");

    // Set parameters
    fov_y_ = XMConvertToRadians(camera_param->fov_y);
    aspect_ratio_ = camera_param->aspect_ratio;
    near_z_ = camera_param->near_z;
    far_z_ = camera_param->far_z;

    // Create graphics service command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = graphics_service_proxy_->CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(command_list.get());
    assert(graphics_command_list != nullptr && "Failed to create GraphicsCommandList");

    // Create view matrix buffer
    graphics_command_list->CreateMatrixBuffer(&view_proj_matrix_buffer_handle_);

    // Create inverse view-projection matrix buffer
    graphics_command_list->CreateMatrixBuffer(&inv_view_proj_matrix_buffer_handle_);

    // Submit command list to graphics service
    graphics_service_proxy_->SubmitCommandList(std::move(command_list));

    return true; // Success
}

bool CameraComponent::Apply(const ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    const CameraComponent::SetupParam* camera_param
        = dynamic_cast<const CameraComponent::SetupParam*>(&param);
    assert(camera_param != nullptr && "Invalid setup param type for CameraComponent");

    // Update parameters
    fov_y_ = XMConvertToRadians(camera_param->fov_y);
    aspect_ratio_ = camera_param->aspect_ratio;
    near_z_ = camera_param->near_z;
    far_z_ = camera_param->far_z;

    return true; // Success
}

ecs::ComponentID CameraComponent::GetID() const
{
    return CameraComponentHandle::ID();
}

} // namespace mono_graphics_extension