#pragma once

#include "component_editor/include/component_reflection_registrar.h"

namespace component_editor_test
{

// Global component reflection registry for tests
extern component_editor::ComponentReflectionRegistry g_component_reflection_registry;

// Registrar for TestMetaComponent reflection info
extern component_editor::ComponentReflectionRegistrar g_test_meta_component_reflection_registrar;

// Registrar for TestTransformComponent reflection info
extern component_editor::ComponentReflectionRegistrar g_test_transform_component_reflection_registrar;

} // namespace component_editor_test