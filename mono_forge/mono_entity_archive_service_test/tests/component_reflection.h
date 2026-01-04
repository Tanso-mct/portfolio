#pragma once

#include "component_editor/include/component_reflection_registrar.h"
#include "component_editor/include/setup_param_editor.h"

namespace component_editor_test
{

// Global component reflection registry for tests
extern component_editor::ComponentReflectionRegistry g_component_reflection_registry;

// Global setup param field value setter for tests
extern component_editor::SetupParamFieldValueSetter g_setup_param_field_value_setter;

} // namespace component_editor_test