#pragma once

#include <unordered_map>
#include <memory>

#include "ecs/include/entity.h"
#include "ecs/include/component.h"

#include "render_graph/include/material.h"
#include "render_graph/include/material_handle.h"

namespace component_editor
{

// The type alias for mapping component IDs to their setup parameters
using ComponentSetupParamMap = std::unordered_map<ecs::ComponentID, std::unique_ptr<ecs::Component::SetupParam>>;

// The type alias for mapping entity IDs to their component setup parameter maps
using EntitySetupParamMap = std::unordered_map<ecs::Entity, ComponentSetupParamMap>;

} // namespace component_editor