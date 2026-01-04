#include "material_editor_test/pch.h"

#include "material_editor/include/setup_param_editor_registry.h"
#include "material_editor/include/setup_param.h"

namespace edit_func_test
{

// Global setup param editor registry
material_editor::SetupParamEditorRegistry<int> g_setup_param_editor_registry;

// Test material type handle ID
constexpr const render_graph::MaterialTypeHandleID TEST_MATERIAL_TYPE_HANDLE_ID = 1;

// Registrar for test material setup param editor
material_editor::SetupParamEditorRegistrar<int> g_test_material_editor_registrar(
    g_setup_param_editor_registry,
    TEST_MATERIAL_TYPE_HANDLE_ID, "TestMaterial",
    [](const material_editor::SetupParamWrapper* setup_param, int num) -> std::unique_ptr<material_editor::SetupParamWrapper>
    {
        std::cout << "Editing test material setup param." << std::endl;
        // Perform editing logic here
        return nullptr; // Indicate not edited
    },
    [](int num) -> bool
    {
        std::cout << "Creating test material" << std::endl;
        // Perform creation logic here
        return false; // Indicate not created
    });

} // namespace edit_func_test

TEST(EditFunc, Call)
{
    // Get the setup param editor function for the test material type handle ID
    material_editor::SetupParamEditFunc<int> edit_func 
        = edit_func_test::g_setup_param_editor_registry.GetSetupParamEditor(
            edit_func_test::TEST_MATERIAL_TYPE_HANDLE_ID);

    // Verify the edit function is valid
    ASSERT_NE(edit_func, nullptr);

    // Create a test material setup param
    std::unique_ptr<render_graph::Material::SetupParam> test_setup_param
        = std::make_unique<render_graph::Material::SetupParam>();

    // Create additional setup param
    std::unique_ptr<material_editor::AdditionalSetupParam> additional_param
        = std::make_unique<material_editor::AdditionalSetupParam>();

    // Create setup param wrapper
    material_editor::SetupParamWrapper test_setup_param_wrapper(
        std::move(test_setup_param),
        std::move(additional_param));

    // Call the edit function
    std::unique_ptr<material_editor::SetupParamWrapper> new_setup_param = edit_func(&test_setup_param_wrapper, 0);

    // Verify the edit function indicates not edited
    ASSERT_EQ(new_setup_param, nullptr);

    // Get the setup param creator function for the test material type handle ID
    material_editor::MaterialCreateFunc<int> create_func
        = edit_func_test::g_setup_param_editor_registry.GetSetupParamCreator(
            edit_func_test::TEST_MATERIAL_TYPE_HANDLE_ID);

    // Verify the create function is valid
    ASSERT_NE(create_func, nullptr);

    // Call the create function
    bool created = create_func(0);

    // Verify the create function indicates not created
    ASSERT_EQ(created, false);
}
