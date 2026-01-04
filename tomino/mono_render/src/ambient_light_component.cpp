#include "mono_render/src/pch.h"
#include "mono_render/include/ambient_light_component.h"

#include "render_graph/include/ambient_light.h"
#include "mono_graphics_service/include/graphics_command_list.h"
#include "mono_adapter/include/service_adapter.h"

mono_render::AmbientLightComponent::AmbientLightComponent()
{
    // Get singleton graphics service adapter
    mono_adapter::GraphicsServiceAdapter& graphics_service_adapter
        = mono_adapter::GraphicsServiceAdapter::GetInstance();
        
    // Create graphics service proxy
    graphics_service_proxy_ = graphics_service_adapter.GetGraphicsService().CreateServiceProxy();
    assert(graphics_service_proxy_ != nullptr);
}

mono_render::AmbientLightComponent::~AmbientLightComponent()
{
    // Create graphics command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = graphics_service_proxy_->CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(command_list.get());
    assert(graphics_command_list != nullptr && "Failed to create graphics service command list");

    // Destroy ambient light in graphics service
    graphics_command_list->DestroyLight(&light_handle_);

    // Submit command list to graphics service
    graphics_service_proxy_->SubmitCommandList(std::move(command_list));
}

void mono_render::AmbientLightComponent::Setup(SetupParam &param)
{
    // Set parameters
    light_color_ = param.light_color;
    intensity_ = param.intensity;

    // Create graphics command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = graphics_service_proxy_->CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(command_list.get());
    assert(graphics_command_list != nullptr && "Failed to create graphics service command list");

    // Create ambient light setup parameters
    std::unique_ptr<render_graph::AmbientLight::SetupParam> resource_setup_param
        = std::make_unique<render_graph::AmbientLight::SetupParam>();
    resource_setup_param->color = light_color_;
    resource_setup_param->intensity = intensity_;

    // Create ambient light in graphics service
    graphics_command_list->CreateAmbientLight(
        &light_handle_, std::move(resource_setup_param), back_buffer_count_);

    // Submit command list to graphics service
    graphics_service_proxy_->SubmitCommandList(std::move(command_list));
}

std::unique_ptr<render_graph::Light::SetupParam> mono_render::AmbientLightComponent::GetLightSetupParam() const
{
    // Create ambient light setup parameter
    std::unique_ptr<render_graph::AmbientLight::SetupParam> setup_param
        = std::make_unique<render_graph::AmbientLight::SetupParam>();

    // Set light parameters
    setup_param->color = light_color_;
    setup_param->intensity = intensity_;

    return setup_param; // Return as Light::SetupParam
}

MONO_RENDER_API riaecs::ComponentRegistrar
<mono_render::AmbientLightComponent, mono_render::AmbientLightComponentMaxCount> mono_render::AmbientLightComponentID;