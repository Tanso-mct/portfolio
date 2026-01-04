#include "mono_graphics_extension/src/pch.h"
#include "mono_graphics_extension/include/ambient_light_component.h"

#include "render_graph/include/ambient_light.h"
#include "mono_graphics_service/include/graphics_command_list.h"

namespace mono_graphics_extension
{

AmbientLightComponent::AmbientLightComponent(
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy, uint32_t back_buffer_count) :
    graphics_service_proxy_(std::move(graphics_service_proxy)),
    back_buffer_count_(back_buffer_count)
{
}

AmbientLightComponent::~AmbientLightComponent()
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

bool AmbientLightComponent::Setup(ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    AmbientLightComponent::SetupParam* ambient_light_component_param
        = dynamic_cast<AmbientLightComponent::SetupParam*>(&param);
    assert(ambient_light_component_param != nullptr && "Invalid setup param type for AmbientLightComponent");

    // Set parameters
    light_color_ = ambient_light_component_param->light_color;
    intensity_ = ambient_light_component_param->intensity;

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

    return true; // Success
}

bool AmbientLightComponent::Apply(const ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    const AmbientLightComponent::SetupParam* ambient_light_component_param
        = dynamic_cast<const AmbientLightComponent::SetupParam*>(&param);
    assert(ambient_light_component_param != nullptr && "Invalid setup param type for AmbientLightComponent");

    // Set parameters
    light_color_ = ambient_light_component_param->light_color;
    intensity_ = ambient_light_component_param->intensity;

    return true; // Success
}

ecs::ComponentID AmbientLightComponent::GetID() const
{
    return AmbientLightComponentHandle::ID();
}

std::unique_ptr<render_graph::Light::SetupParam> AmbientLightComponent::GetLightSetupParam() const
{
    // Create ambient light setup parameters
    std::unique_ptr<render_graph::AmbientLight::SetupParam> setup_param
        = std::make_unique<render_graph::AmbientLight::SetupParam>();
    setup_param->color = light_color_;
    setup_param->intensity = intensity_;

    return setup_param;
}

} // namespace mono_graphics_extension