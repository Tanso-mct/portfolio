#pragma once

#include "component_editor/include/component_reflection_registrar.h"
#include "component_editor/include/setup_param_editor.h"
#include "mono_entity_archive_service/include/entity_archive_service.h"

namespace component_editor_test
{

// Global component reflection registry for tests
extern component_editor::ComponentReflectionRegistry g_component_reflection_registry;

// Global setup param field value setter for tests
extern component_editor::SetupParamFieldValueSetter g_setup_param_field_value_setter;

// Global setup param field type registry for tests
extern mono_entity_archive_service::ComponentSetupParamFieldTypeRegistry g_setup_param_field_type_registry;


} // namespace component_editor_test