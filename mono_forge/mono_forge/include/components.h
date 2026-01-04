#pragma once

#include "component_editor/include/component_reflection_registry.h"
#include "component_editor/include/component_adder.h"
#include "component_editor/include/setup_param_field_type.h"
#include "component_editor/include/setup_param_editor.h"
#include "mono_entity_archive_service/include/entity_archive_service.h"

namespace mono_forge
{

// Global component ID generator
extern ecs::ComponentIDGenerator g_component_id_generator;

// Global component reflection registry
extern component_editor::ComponentReflectionRegistry g_component_reflection_registry;

// Global setup param field type registry
extern mono_entity_archive_service::ComponentSetupParamFieldTypeRegistry g_setup_param_field_type_registry;

// Global component field value setter
extern component_editor::SetupParamFieldValueSetter g_component_field_value_setter;

// Global component name map
extern component_editor::ComponentNameMap g_component_name_map;

// Global component adder map
extern component_editor::ComponentAdderMap g_component_adder_map;

// The global component descriptor registry
extern ecs::ComponentDescriptorRegistry g_component_descriptor_registry;

} // namespace mono_forge