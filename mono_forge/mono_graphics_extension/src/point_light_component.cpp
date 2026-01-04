#include "mono_graphics_extension/src/pch.h"
#include "mono_graphics_extension/include/point_light_component.h"

#include "render_graph/include/point_light.h"
#include "mono_graphics_service/include/graphics_command_list.h"

namespace mono_graphics_extension
{

PointLightComponent::PointLightComponent(
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy, uint32_t back_buffer_count) :
    graphics_service_proxy_(std::move(graphics_service_proxy)),
    back_buffer_count_(back_buffer_count)
{
}

PointLightComponent::~PointLightComponent()
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

bool PointLightComponent::Setup(ecs::Component::SetupParam& param)
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

    return true; // Success
}

bool PointLightComponent::Apply(const ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    const PointLightComponent::SetupParam* point_light_component_param
        = dynamic_cast<const PointLightComponent::SetupParam*>(&param);
    assert(point_light_component_param != nullptr && "Invalid setup param type for PointLightComponent");

    // Set parameters
    light_color_ = point_light_component_param->light_color;
    intensity_ = point_light_component_param->intensity;
    range_ = point_light_component_param->range;

    return true; // Success
}

ecs::ComponentID PointLightComponent::GetID() const
{
    return PointLightComponentHandle::ID();
}

std::unique_ptr<render_graph::Light::SetupParam> PointLightComponent::GetLightSetupParam() const
{
    // Create point light setup parameters
    std::unique_ptr<render_graph::PointLight::SetupParam> resource_setup_param
        = std::make_unique<render_graph::PointLight::SetupParam>();
    resource_setup_param->color = light_color_;
    resource_setup_param->intensity = intensity_;
    resource_setup_param->range = range_;

    return resource_setup_param;
}

} // namespace mono_graphics_extension