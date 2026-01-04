#include "mono_scene_extension/src/pch.h"
#include "mono_scene_extension/include/scene_tag_component.h"

namespace mono_scene_extension
{

SceneTagComponent::SceneTagComponent()
{
}

SceneTagComponent::~SceneTagComponent()
{
}

bool SceneTagComponent::Setup(ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    SceneTagComponent::SetupParam* scene_tag_component_param
        = dynamic_cast<SceneTagComponent::SetupParam*>(&param);
    assert(scene_tag_component_param != nullptr && "Invalid setup param type for SceneTagComponent");

    // Set parameters
    scene_id_ = scene_tag_component_param->scene_id;

    return true; // Success
}

ecs::ComponentID SceneTagComponent::GetID() const
{
    return SceneTagComponentHandle::ID();
}

} // namespace mono_scene_extension