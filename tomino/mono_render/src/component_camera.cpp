#include "mono_render/src/pch.h"
#include "mono_render/include/component_camera.h"

#include "mono_graphics_service/include/graphics_command_list.h"
#include "mono_adapter/include/service_adapter.h"

mono_render::ComponentCamera::ComponentCamera()
{
    // Get singleton graphics service adapter
    mono_adapter::GraphicsServiceAdapter& graphics_service_adapter
        = mono_adapter::GraphicsServiceAdapter::GetInstance();
        
    // Create graphics service proxy
    graphics_service_proxy_ = graphics_service_adapter.GetGraphicsService().CreateServiceProxy();
    assert(graphics_service_proxy_ != nullptr);
}

mono_render::ComponentCamera::~ComponentCamera()
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

void mono_render::ComponentCamera::Setup(SetupParam &param)
{
    fov_y_ = param.fov_y;
    near_z_ = param.nearZ;
    far_z_ = param.farZ;
    aspect_ratio_ = param.aspect_ratio;

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
}

MONO_RENDER_API riaecs::ComponentRegistrar
<mono_render::ComponentCamera, mono_render::ComponentCameraMaxCount> mono_render::ComponentCameraID;