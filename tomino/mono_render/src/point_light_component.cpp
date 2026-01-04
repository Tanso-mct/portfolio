#include "mono_render/src/pch.h"
#include "mono_render/include/point_light_component.h"

#include "render_graph/include/point_light.h"
#include "mono_graphics_service/include/graphics_command_list.h"
#include "mono_adapter/include/service_adapter.h"

mono_render::PointLightComponent::PointLightComponent()
{
    // Get singleton graphics service adapter
    mono_adapter::GraphicsServiceAdapter& graphics_service_adapter
        = mono_adapter::GraphicsServiceAdapter::GetInstance();
        
    // Create graphics service proxy
    graphics_service_proxy_ = graphics_service_adapter.GetGraphicsService().CreateServiceProxy();
    assert(graphics_service_proxy_ != nullptr);
}

mono_render::PointLightComponent::~PointLightComponent()
{
    // Create graphics command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = graphics_service_proxy_->CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(command_list.get());
    assert(graphics_command_list != nullptr && "Failed to create graphics service command list");

    // Destroy point light in graphics service
    graphics_command_list->DestroyLight(&light_handle_);

    // Submit command list to graphics service
    graphics_service_proxy_->SubmitCommandList(std::move(command_list));
}

void mono_render::PointLightComponent::Setup(SetupParam &param)
{
    // Dynamic cast to SetupParam
    PointLightComponent::SetupParam* point_light_component_param
        = dynamic_cast<PointLightComponent::SetupParam*>(&param);
    assert(point_light_component_param != nullptr && "Invalid setup param type for PointLightComponent");

    // Set parameters
    light_color_ = point_light_component_param->light_color;
    intensity_ = point_light_component_param->intensity;
    range_ = point_light_component_param->range;

    // Create graphics command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = graphics_service_proxy_->CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(command_list.get());
    assert(graphics_command_list != nullptr && "Failed to create graphics service command list");

    // Create point light setup parameters
    std::unique_ptr<render_graph::PointLight::SetupParam> resource_setup_param
        = std::make_unique<render_graph::PointLight::SetupParam>();
    resource_setup_param->color = light_color_;
    resource_setup_param->intensity = intensity_;
    resource_setup_param->range = range_;

    // Create point light in graphics service
    graphics_command_list->CreatePointLight(
        &light_handle_, std::move(resource_setup_param), back_buffer_count_);

    // Submit command list to graphics service
    graphics_service_proxy_->SubmitCommandList(std::move(command_list));
}

std::unique_ptr<render_graph::Light::SetupParam> mono_render::PointLightComponent::GetLightSetupParam() const
{
    // Create point light setup parameters
    std::unique_ptr<render_graph::PointLight::SetupParam> resource_setup_param
        = std::make_unique<render_graph::PointLight::SetupParam>();
    resource_setup_param->color = light_color_;
    resource_setup_param->intensity = intensity_;
    resource_setup_param->range = range_;

    return resource_setup_param;
}

MONO_RENDER_API riaecs::ComponentRegistrar
<mono_render::PointLightComponent, mono_render::PointLightComponentMaxCount> mono_render::PointLightComponentID;