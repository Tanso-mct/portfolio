#include "component_editor_test/pch.h"

#include "component_editor/include/component_adder.h"
#include "component_editor_test/tests/component_reflection.h"
#include "component_editor_test/tests/test_meta_component.h"
#include "component_editor_test/tests/test_transform_component.h"

TEST(ReflectionLoader, LoadReflectionInfoFromJSON)
{
    // Create component id generator
    std::unique_ptr<ecs::ComponentIDGenerator> component_id_generator 
        = std::make_unique<ecs::ComponentIDGenerator>();

    // Create component id to name map
    component_editor::ComponentNameMap component_name_map;

    // Register component IDs and names
    component_name_map[
        component_editor_test::TestMetaComponentHandle::ID()] = "component_editor_test::TestMetaComponent";
    component_name_map[
        component_editor_test::TestTransformComponentHandle::ID()] = "component_editor_test::TestTransformComponent";

    // Create component adder map
    component_editor::ComponentAdderMap component_adder_map;

    // Register component adders
    component_adder_map[component_editor_test::TestMetaComponentHandle::ID()] 
        = std::make_unique<component_editor_test::TestMetaComponentAdder>();
    component_adder_map[component_editor_test::TestTransformComponentHandle::ID()] 
        = std::make_unique<component_editor_test::TestTransformComponentAdder>();

    // Check that the expected number of components were loaded
    EXPECT_EQ(
        component_editor_test::g_component_reflection_registry.size(), 2);

    std::cout << "Loaded Component Reflection Infos:" << std::endl;

    for (const auto&[component_id, component_name] : component_name_map)
    {
        // Check that each component ID has a corresponding reflection info
        auto it = component_editor_test::g_component_reflection_registry.find(
            component_editor::ComponentReflectionInfo(component_name, {}));
        EXPECT_NE(it, component_editor_test::g_component_reflection_registry.end());

        // Get the reflection info
        const component_editor::ComponentReflectionInfo& reflection_info = *it;

        std::cout << "Component Name: " << reflection_info.GetComponentName() << std::endl;

        // Verify fields of this component
        for (const auto&[field_name, field_info] : reflection_info.GetFieldMap())
        {
            std::cout 
            << "Field Name: " << field_name
            << ", Type: " << field_info.type_name
            << ", Offset: " << field_info.offset << std::endl;
        }
    }
}