#include "mono_scene_extension/src/pch.h"
#include "mono_scene_extension/include/scene_component.h"

namespace mono_scene_extension
{

SceneComponent::SceneComponent(SceneID scene_id, std::unique_ptr<Scene> scene) :
    scene_id_(scene_id),
    scene_(std::move(scene))
{
    assert(scene_ != nullptr && "Scene instance must not be null");
}

SceneComponent::~SceneComponent()
{
}

bool SceneComponent::Setup(ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    SceneComponent::SetupParam* scene_component_param
        = dynamic_cast<SceneComponent::SetupParam*>(&param);
    assert(scene_component_param != nullptr && "Invalid setup param type for SceneComponent");

    assert(!scene_component_param->required_window_component_ids.empty() && "Required window component IDs is empty");
    required_window_component_ids_ = std::move(scene_component_param->required_window_component_ids);

    assert(!scene_component_param->required_camera_component_ids.empty() && "Required camera component IDs is empty");
    required_camera_component_ids_ = std::move(scene_component_param->required_camera_component_ids);

    assert(!scene_component_param->required_renderable_component_ids.empty() && "Required renderable component IDs is empty");
    required_renderable_component_ids_ = std::move(scene_component_param->required_renderable_component_ids);

    assert(!scene_component_param->required_ui_component_ids.empty() && "Required UI component IDs is empty");
    required_ui_component_ids_ = std::move(scene_component_param->required_ui_component_ids);

    assert(!scene_component_param->light_component_ids.empty() && "Light component IDs is empty");
    light_component_ids_ = std::move(scene_component_param->light_component_ids);

    clear_color_ = scene_component_param->clear_color;
    max_light_count_ = scene_component_param->max_light_count;
    shadow_bias_ = scene_component_param->shadow_bias;
    shadow_slope_scaled_bias_ = scene_component_param->shadow_slope_scaled_bias;
    shadow_slope_bias_exponent_ = scene_component_param->shadow_slope_bias_exponent;
    shadow_intensity_ = scene_component_param->shadow_intensity;
    current_state_ = scene_component_param->initial_state;

    return true; // Success
}

bool SceneComponent::Apply(const ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    const SceneComponent::SetupParam* scene_component_param
        = dynamic_cast<const SceneComponent::SetupParam*>(&param);
    assert(scene_component_param != nullptr && "Invalid setup param type for SceneComponent");

    assert(!scene_component_param->required_window_component_ids.empty() && "Required window component IDs is empty");
    required_window_component_ids_ = scene_component_param->required_window_component_ids;

    assert(!scene_component_param->required_camera_component_ids.empty() && "Required camera component IDs is empty");
    required_camera_component_ids_ = scene_component_param->required_camera_component_ids;

    assert(!scene_component_param->required_renderable_component_ids.empty() && "Required renderable component IDs is empty");
    required_renderable_component_ids_ = scene_component_param->required_renderable_component_ids;

    assert(!scene_component_param->required_ui_component_ids.empty() && "Required UI component IDs is empty");
    required_ui_component_ids_ = scene_component_param->required_ui_component_ids;

    assert(!scene_component_param->light_component_ids.empty() && "Light component IDs is empty");
    light_component_ids_ = scene_component_param->light_component_ids;

    clear_color_ = scene_component_param->clear_color;
    max_light_count_ = scene_component_param->max_light_count;
    shadow_bias_ = scene_component_param->shadow_bias;
    shadow_slope_scaled_bias_ = scene_component_param->shadow_slope_scaled_bias;
    shadow_slope_bias_exponent_ = scene_component_param->shadow_slope_bias_exponent;
    shadow_intensity_ = scene_component_param->shadow_intensity;

    return true; // Success
}

ecs::ComponentID SceneComponent::GetID() const
{
    return SceneComponentHandle::ID();
}

void SceneComponent::SetStateToNeedsLoad()
{
    assert(
        current_state_ == SceneState::Released &&
        "Can only set state to NeedsLoad from Released state");

    // Set current state to NeedsLoad
    current_state_ = SceneState::NeedsLoad;
}

void SceneComponent::SetStateToNeedsRelease()
{
    assert(
        current_state_ == SceneState::Playing &&
        "Can only set state to NeedsRelease from Playing state");

    // Set current state to NeedsRelease
    current_state_ = SceneState::NeedsRelease;
}

} // namespace mono_scene_extension