#include "mono_entity_archive_extension/src/pch.h"
#include "mono_entity_archive_extension/include/project_io_component.h"

namespace mono_entity_archive_extension
{

ProjectIOComponent::ProjectIOComponent()
{
}

ProjectIOComponent::~ProjectIOComponent()
{
}

bool ProjectIOComponent::Setup(ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    ProjectIOComponent::SetupParam* project_io_component_param
        = dynamic_cast<ProjectIOComponent::SetupParam*>(&param);
    assert(project_io_component_param != nullptr && "Invalid setup param type for ProjectIOComponent");

    // Set parameters


    return true; // Success
}

bool ProjectIOComponent::Apply(const ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    const ProjectIOComponent::SetupParam* project_io_component_param
        = dynamic_cast<const ProjectIOComponent::SetupParam*>(&param);
    assert(project_io_component_param != nullptr && "Invalid setup param type for ProjectIOComponent");

    // Set parameters

    return true; // Success
}

ecs::ComponentID ProjectIOComponent::GetID() const
{
    return ProjectIOComponentHandle::ID();
}

} // namespace mono_entity_archive_extension