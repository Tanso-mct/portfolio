#include "mono_render/src/pch.h"
#include "mono_render/include/directional_light_component.h"

#include "render_graph/include/directional_light.h"
#include "mono_graphics_service/include/graphics_command_list.h"
#include "mono_adapter/include/service_adapter.h"

mono_render::DirectionalLightComponent::DirectionalLightComponent()
{
    // Get singleton graphics service adapter
    mono_adapter::GraphicsServiceAdapter& graphics_service_adapter
        = mono_adapter::GraphicsServiceAdapter::GetInstance();
        
    // Create graphics service proxy
    graphics_service_proxy_ = graphics_service_adapter.GetGraphicsService().CreateServiceProxy();
    assert(graphics_service_proxy_ != nullptr);
}

mono_render::DirectionalLightComponent::~DirectionalLightComponent()
{
    // Create graphics command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = graphics_service_proxy_->CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(command_list.get());
    assert(graphics_command_list != nullptr && "Failed to create graphics service command list");

    // Destroy directional light in graphics service
    graphics_command_list->DestroyLight(&light_handle_);

    // Submit command list to graphics service
    graphics_service_proxy_->SubmitCommandList(std::move(command_list));
}

void mono_render::DirectionalLightComponent::Setup(SetupParam &param)
{
    // Set parameters
    light_color_ = param.light_color;
    intensity_ = param.intensity;
    distance_ = param.distance;
    ortho_width_ = param.ortho_width;
    ortho_height_ = param.ortho_height;
    near_z_ = param.near_z;
    far_z_ = param.far_z;
    shadow_map_size_ = param.shadow_map_size;

    // Create graphics command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = graphics_service_proxy_->CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(command_list.get());
    assert(graphics_command_list != nullptr && "Failed to create graphics service command list");

    // Create directional light setup parameters
    std::unique_ptr<render_graph::DirectionalLight::SetupParam> resource_setup_param
        = std::make_unique<render_graph::DirectionalLight::SetupParam>();
    resource_setup_param->color = light_color_;
    resource_setup_param->intensity = intensity_;
    resource_setup_param->distance = distance_;
    resource_setup_param->ortho_width = ortho_width_;
    resource_setup_param->ortho_height = ortho_height_;
    resource_setup_param->near_z = near_z_;
    resource_setup_param->far_z = far_z_;
    resource_setup_param->shadow_map_size = shadow_map_size_;
    
    // Create directional light in graphics service
    graphics_command_list->CreateDirectionalLight(
        &light_handle_, std::move(resource_setup_param), back_buffer_count_);

    // Submit command list to graphics service
    graphics_service_proxy_->SubmitCommandList(std::move(command_list));
}

std::unique_ptr<render_graph::Light::SetupParam> mono_render::DirectionalLightComponent::GetLightSetupParam() const
{
    // Create directional light setup parameter
    std::unique_ptr<render_graph::DirectionalLight::SetupParam> setup_param
        = std::make_unique<render_graph::DirectionalLight::SetupParam>();

    // Set light parameters
    setup_param->color = light_color_;
    setup_param->intensity = intensity_;
    setup_param->distance = distance_;
    setup_param->ortho_width = ortho_width_;
    setup_param->ortho_height = ortho_height_;
    setup_param->near_z = near_z_;
    setup_param->far_z = far_z_;
    setup_param->shadow_map_size = shadow_map_size_;

    return setup_param; // Return as Light::SetupParam
}

MONO_RENDER_API riaecs::ComponentRegistrar
<mono_render::DirectionalLightComponent, mono_render::DirectionalLightComponentMaxCount> mono_render::DirectionalLightComponentID;