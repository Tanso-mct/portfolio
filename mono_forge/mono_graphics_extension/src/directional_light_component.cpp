#include "mono_graphics_extension/src/pch.h"
#include "mono_graphics_extension/include/directional_light_component.h"

#include "render_graph/include/directional_light.h"
#include "mono_graphics_service/include/graphics_command_list.h"

namespace mono_graphics_extension
{

DirectionalLightComponent::DirectionalLightComponent(
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy_, uint32_t back_buffer_count) :
    graphics_service_proxy_(std::move(graphics_service_proxy_)),
    back_buffer_count_(back_buffer_count)
{
}

DirectionalLightComponent::~DirectionalLightComponent()
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

bool DirectionalLightComponent::Setup(ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    DirectionalLightComponent::SetupParam* directional_light_component_param
        = dynamic_cast<DirectionalLightComponent::SetupParam*>(&param);
    assert(directional_light_component_param != nullptr && "Invalid setup param type for DirectionalLightComponent");

    // Set parameters
    light_color_ = directional_light_component_param->light_color;
    intensity_ = directional_light_component_param->intensity;
    distance_ = directional_light_component_param->distance;
    ortho_width_ = directional_light_component_param->ortho_width;
    ortho_height_ = directional_light_component_param->ortho_height;
    near_z_ = directional_light_component_param->near_z;
    far_z_ = directional_light_component_param->far_z;
    shadow_map_size_ = directional_light_component_param->shadow_map_size;

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

    return true; // Success
}

bool DirectionalLightComponent::Apply(const ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    const DirectionalLightComponent::SetupParam* directional_light_component_param
        = dynamic_cast<const DirectionalLightComponent::SetupParam*>(&param);
    assert(directional_light_component_param != nullptr && "Invalid setup param type for DirectionalLightComponent");

    // Set parameters
    light_color_ = directional_light_component_param->light_color;
    intensity_ = directional_light_component_param->intensity;
    distance_ = directional_light_component_param->distance;
    ortho_width_ = directional_light_component_param->ortho_width;
    ortho_height_ = directional_light_component_param->ortho_height;
    near_z_ = directional_light_component_param->near_z;
    far_z_ = directional_light_component_param->far_z;
    shadow_map_size_ = shadow_map_size_;

    return true; // Success
}

ecs::ComponentID DirectionalLightComponent::GetID() const
{
    return DirectionalLightComponentHandle::ID();
}

std::unique_ptr<render_graph::Light::SetupParam> DirectionalLightComponent::GetLightSetupParam() const
{
    // Create directional light setup parameters
    std::unique_ptr<render_graph::DirectionalLight::SetupParam> setup_param
        = std::make_unique<render_graph::DirectionalLight::SetupParam>();
    setup_param->color = light_color_;
    setup_param->intensity = intensity_;
    setup_param->distance = distance_;
    setup_param->ortho_width = ortho_width_;
    setup_param->ortho_height = ortho_height_;
    setup_param->near_z = near_z_;
    setup_param->far_z = far_z_;
    setup_param->shadow_map_size = shadow_map_size_;

    return setup_param;
}

} // namespace mono_graphics_extension