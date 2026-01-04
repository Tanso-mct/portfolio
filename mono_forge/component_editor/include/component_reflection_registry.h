#pragma once

#include <string>
#include <unordered_set>

#include "ecs/include/component.h"
#include "component_editor/include/component_reflection_info.h"

namespace component_editor
{

// Registry for component reflection information
using ComponentReflectionRegistry = std::unordered_set<ComponentReflectionInfo>;

// Map from component ID to component name
using ComponentNameMap = std::unordered_map<ecs::ComponentID, std::string>;

} // namespace component_editor