#include "material_editor_test/pch.h"

#include "material_editor/include/setup_param_manager.h"

TEST(Manager, Use)
{
    // Create setup param map
    std::unique_ptr<material_editor::SetupParamMap> setup_param_map 
        = std::make_unique<material_editor::SetupParamMap>();

    // Create SetupParamManager
    std::unique_ptr<material_editor::SetupParamManager> setup_param_manager 
        = std::make_unique<material_editor::SetupParamManager>(*setup_param_map);

    // Create SetupParamAdder
    std::unique_ptr<material_editor::SetupParamAdder> setup_param_adder 
        = std::make_unique<material_editor::SetupParamAdder>(*setup_param_map);

    // Create SetupParamEraser
    std::unique_ptr<material_editor::SetupParamEraser> setup_param_eraser 
        = std::make_unique<material_editor::SetupParamEraser>(*setup_param_map);

    // Create test material handle
    render_graph::MaterialHandle test_material_handle = render_graph::MaterialHandle(1, 0);

    {
        // Create test material setup param
        std::unique_ptr<render_graph::Material::SetupParam> material_setup_param 
            = std::make_unique<render_graph::Material::SetupParam>();

        std::unique_ptr<material_editor::AdditionalSetupParam> additional_setup_param 
            = std::make_unique<material_editor::AdditionalSetupParam>();

        std::unique_ptr<material_editor::SetupParamWrapper> setup_param_wrapper
            = std::make_unique<material_editor::SetupParamWrapper>(
                std::move(material_setup_param),
                std::move(additional_setup_param));

        // Add setup param to setup param registry
        setup_param_adder->AddSetupParam(
            &test_material_handle, std::move(setup_param_wrapper));
    }

    // Get and verify test material setup param
    {
        material_editor::SetupParamWrapper* setup_param = setup_param_manager->GetSetupParam(&test_material_handle);
        ASSERT_NE(setup_param, nullptr);
    }

    // Erase test material setup param
    setup_param_eraser->EraseSetupParam(test_material_handle);

    // Verify test material setup param is erased
    {
        bool contains = false;

        contains = setup_param_manager->ContainSetupParam(&test_material_handle);
        EXPECT_FALSE(contains);
    }
}