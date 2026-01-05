#include "component_editor_test/pch.h"

using namespace DirectX;

#include "component_editor/include/setup_param_manager.h"
#include "component_editor/include/setup_param_editor.h"
#include "component_editor/include/setup_param_field_type.h"

#include "component_editor_test/tests/component_reflection.h"
#include "component_editor_test/tests/test_meta_component.h"
#include "component_editor_test/tests/test_transform_component.h"

namespace component_editor_test
{

component_editor::SetupParamFieldTypeRegistry<int> g_setup_param_field_type_registry;

component_editor::SetupParamFieldTypeRegistrar<int> g_string_field_type_registrar(
    g_setup_param_field_type_registry,
    "std::string",
    [](const uint8_t* field_value, int num) -> std::any
    {
        std::string value = *reinterpret_cast<const std::string*>(field_value);
        return value;
    },
    [](std::any& value, std::string_view field_name, int num) -> bool
    {
        std::string new_value;
        std::cout << "Field: " << field_name << " Enter new string value: ";
        std::getline(std::cin, new_value);
        value = new_value;
        return true;
    },
    [](const std::any& field_value, std::string_view field_name, int num) -> nlohmann::json
    {
        return std::any_cast<const std::string&>(field_value);
    },
    [](std::string_view field_name, const nlohmann::json& json, int num) -> std::any
    {
        std::string value;
        assert(json.contains(field_name.data()) && "JSON does not contain the field name.");
        value = json[field_name.data()].get<std::string>();
        return value;
    });

component_editor::SetupParamFieldTypeRegistrar<int> g_bool_field_type_registrar(
    g_setup_param_field_type_registry,
    "bool",
    [](const uint8_t* field_value, int num) -> std::any
    {
        bool value = *reinterpret_cast<const bool*>(field_value);
        return value;
    },
    [](std::any& value, std::string_view field_name, int num) -> bool
    {
        bool new_value;
        std::cout << "Field: " << field_name << " Enter new bool value (0 or 1): ";
        std::cin >> new_value;
        value = new_value;
        return true;
    },
    [](const std::any& field_value, std::string_view field_name, int num) -> nlohmann::json
    {
        return std::any_cast<const bool&>(field_value);
    },
    [](std::string_view field_name, const nlohmann::json& json, int num) -> std::any
    {
        bool value = false;
        value = json[field_name.data()].get<bool>();
        return value;
    });

component_editor::SetupParamFieldValueSetter g_setup_param_field_value_setter;

component_editor::SetupParamFieldValueSetter::Registrar<std::string> g_string_field_value_setter_registrar(
    g_setup_param_field_value_setter, "std::string");

component_editor::SetupParamFieldValueSetter::Registrar<bool> g_bool_field_value_setter_registrar(
    g_setup_param_field_value_setter, "bool");

component_editor::SetupParamFieldValueSetter::Registrar<uint64_t> g_uint64_field_value_setter_registrar(
    g_setup_param_field_value_setter, "uint64_t");

component_editor::SetupParamFieldValueSetter::Registrar<XMFLOAT3> g_xmfloat3_field_value_setter_registrar(
    g_setup_param_field_value_setter, "DirectX::XMFLOAT3");

} // namespace component_editor_test

TEST(SetupParam, AddGetEraseSetupParam)
{
    // Create entity setup param map
    std::unique_ptr<component_editor::EntitySetupParamMap> entity_setup_param_map 
        = std::make_unique<component_editor::EntitySetupParamMap>();

    // Create SetupParamManager
    std::unique_ptr<component_editor::SetupParamManager> setup_param_manager 
        = std::make_unique<component_editor::SetupParamManager>(*entity_setup_param_map);

    // Create SetupParamAdder
    std::unique_ptr<component_editor::SetupParamAdder> setup_param_adder 
        = std::make_unique<component_editor::SetupParamAdder>(*entity_setup_param_map);

    // Create SetupParamEraser
    std::unique_ptr<component_editor::SetupParamEraser> setup_param_eraser 
        = std::make_unique<component_editor::SetupParamEraser>(*entity_setup_param_map);

    // Create component id generator
    std::unique_ptr<ecs::ComponentIDGenerator> component_id_generator 
        = std::make_unique<ecs::ComponentIDGenerator>();

    // Create test entity
    ecs::Entity test_entity(0, 0);

    {
        // Create test meta component setup param
        std::unique_ptr<component_editor_test::TestMetaComponent::SetupParam> meta_setup_param 
            = std::make_unique<component_editor_test::TestMetaComponent::SetupParam>();

        // Add setup param to setup param registry
        setup_param_adder->AddSetupParam(
            test_entity, component_editor_test::TestMetaComponentHandle::ID(), std::move(meta_setup_param));
    }

    {
        // Create test transform component setup param
        std::unique_ptr<component_editor_test::TestTransformComponent::SetupParam> transform_setup_param 
            = std::make_unique<component_editor_test::TestTransformComponent::SetupParam>();

        // Add setup param to setup param registry
        setup_param_adder->AddSetupParam(
            test_entity, component_editor_test::TestTransformComponentHandle::ID(), std::move(transform_setup_param));
    }

    // Get and verify test meta component setup param
    {
        ecs::Component::SetupParam* setup_param = setup_param_manager->GetSetupParam(
            test_entity, component_editor_test::TestMetaComponentHandle::ID());
        ASSERT_NE(setup_param, nullptr);

        component_editor_test::TestMetaComponent::SetupParam* meta_setup_param = 
            dynamic_cast<component_editor_test::TestMetaComponent::SetupParam*>(setup_param);
        ASSERT_NE(meta_setup_param, nullptr);
    }

    // Get and verify test transform component setup param
    {
        ecs::Component::SetupParam* setup_param = setup_param_manager->GetSetupParam(
            test_entity, component_editor_test::TestTransformComponentHandle::ID());
        ASSERT_NE(setup_param, nullptr);

        component_editor_test::TestTransformComponent::SetupParam* transform_setup_param = 
            dynamic_cast<component_editor_test::TestTransformComponent::SetupParam*>(setup_param);
        ASSERT_NE(transform_setup_param, nullptr);
    }

    // Erase test meta component setup param
    setup_param_eraser->EraseSetupParam(
        test_entity, component_editor_test::TestMetaComponentHandle::ID());

    // Verify test meta component setup param is erased
    {
        bool contains = false;

        contains = setup_param_manager->ContainSetupParam(
            test_entity, component_editor_test::TestMetaComponentHandle::ID());
        EXPECT_FALSE(contains);

        contains = setup_param_manager->ContainSetupParam(
            test_entity, component_editor_test::TestTransformComponentHandle::ID());
        EXPECT_TRUE(contains);
    }

    // Erase test transform component setup param
    setup_param_eraser->EraseSetupParam(
        test_entity, component_editor_test::TestTransformComponentHandle::ID());

    // Verify test transform component setup param is erased
    {
        bool contains = false;

        contains = setup_param_manager->ContainSetupParam(
            test_entity, component_editor_test::TestMetaComponentHandle::ID());
        EXPECT_FALSE(contains);

        contains = setup_param_manager->ContainSetupParam(
            test_entity, component_editor_test::TestTransformComponentHandle::ID());
        EXPECT_FALSE(contains);
    }
}

TEST(SetupParam, Editor)
{
    // Create entity setup param map
    std::unique_ptr<component_editor::EntitySetupParamMap> entity_setup_param_map 
        = std::make_unique<component_editor::EntitySetupParamMap>();

    // Create SetupParamManager
    std::unique_ptr<component_editor::SetupParamManager> setup_param_manager 
        = std::make_unique<component_editor::SetupParamManager>(*entity_setup_param_map);

    // Create SetupParamAdder
    std::unique_ptr<component_editor::SetupParamAdder> setup_param_adder 
        = std::make_unique<component_editor::SetupParamAdder>(*entity_setup_param_map);

    // Create SetupParamEraser
    std::unique_ptr<component_editor::SetupParamEraser> setup_param_eraser 
        = std::make_unique<component_editor::SetupParamEraser>(*entity_setup_param_map);

    // Create component id generator
    std::unique_ptr<ecs::ComponentIDGenerator> component_id_generator 
        = std::make_unique<ecs::ComponentIDGenerator>();

    // Create component id to name map
    component_editor::ComponentNameMap component_id_to_name_map;

    // Register component IDs and names
    component_id_to_name_map[
        component_editor_test::TestMetaComponentHandle::ID()] = "component_editor_test::TestMetaComponent";
    component_id_to_name_map[
        component_editor_test::TestTransformComponentHandle::ID()] = "component_editor_test::TestTransformComponent";

    // Create setup param editor
    std::unique_ptr<component_editor::SetupParamEditor> setup_param_editor
        = std::make_unique<component_editor::SetupParamEditor>(
            *setup_param_manager,
            component_id_to_name_map,
            component_editor_test::g_component_reflection_registry,
            std::move(component_editor_test::g_setup_param_field_value_setter));

    // Create test entity
    ecs::Entity test_entity(0, 0);

    {
        // Create test meta component setup param
        std::unique_ptr<component_editor_test::TestMetaComponent::SetupParam> meta_setup_param 
            = std::make_unique<component_editor_test::TestMetaComponent::SetupParam>();

        // Add setup param to setup param registry
        setup_param_adder->AddSetupParam(
            test_entity, component_editor_test::TestMetaComponentHandle::ID(), std::move(meta_setup_param));
    }

    {
        // Create test transform component setup param
        std::unique_ptr<component_editor_test::TestTransformComponent::SetupParam> transform_setup_param 
            = std::make_unique<component_editor_test::TestTransformComponent::SetupParam>();

        // Add setup param to setup param registry
        setup_param_adder->AddSetupParam(
            test_entity, component_editor_test::TestTransformComponentHandle::ID(), std::move(transform_setup_param));
    }

    // Check if test entity's meta component setup param is editable
    {
        bool is_editable = setup_param_editor->IsEditable(
            test_entity, component_editor_test::TestMetaComponentHandle::ID());
        EXPECT_TRUE(is_editable);
    }

    // Edit meta component setup param fields
    constexpr const char* EDITED_NAME = "EditedEntity";
    constexpr bool EDITED_ACTIVE_SELF = false;
    constexpr uint64_t EDITED_TAG = 42;
    constexpr uint64_t EDITED_LAYER = 7;
    {
        std::string new_name = EDITED_NAME;
        setup_param_editor->Edit(
            test_entity, component_editor_test::TestMetaComponentHandle::ID(), "name", new_name);

        bool new_active_self = EDITED_ACTIVE_SELF;
        setup_param_editor->Edit(
            test_entity, component_editor_test::TestMetaComponentHandle::ID(), "active_self", new_active_self);

        uint64_t new_tag = EDITED_TAG;
        setup_param_editor->Edit(
            test_entity, component_editor_test::TestMetaComponentHandle::ID(), "tag", new_tag);

        uint64_t new_layer = EDITED_LAYER;
        setup_param_editor->Edit(
            test_entity, component_editor_test::TestMetaComponentHandle::ID(), "layer", new_layer);
    }

    // Check if test entity's transform component setup param is editable
    {
        bool is_editable = setup_param_editor->IsEditable(
            test_entity, component_editor_test::TestTransformComponentHandle::ID());
        EXPECT_TRUE(is_editable);
    }

    // Edit transform component setup param fields
    constexpr XMFLOAT3 EDITED_POSITION(1.0f, 2.0f, 3.0f);
    constexpr XMFLOAT3 EDITED_ROTATION(10.0f, 20.0f, 30.0f);
    constexpr XMFLOAT3 EDITED_SCALE(2.0f, 2.0f, 2.0f);
    {
        XMFLOAT3 new_position = EDITED_POSITION;
        setup_param_editor->Edit(
            test_entity, component_editor_test::TestTransformComponentHandle::ID(), "position", new_position);

        XMFLOAT3 new_rotation = EDITED_ROTATION;
        setup_param_editor->Edit(
            test_entity, component_editor_test::TestTransformComponentHandle::ID(), "rotation", new_rotation);

        XMFLOAT3 new_scale = EDITED_SCALE;
        setup_param_editor->Edit(
            test_entity, component_editor_test::TestTransformComponentHandle::ID(), "scale", new_scale);
    }

    // Verify edited meta component setup param fields
    {
        ecs::Component::SetupParam* setup_param = setup_param_manager->GetSetupParam(
            test_entity, component_editor_test::TestMetaComponentHandle::ID());
        ASSERT_NE(setup_param, nullptr);

        component_editor_test::TestMetaComponent::SetupParam* meta_setup_param = 
            dynamic_cast<component_editor_test::TestMetaComponent::SetupParam*>(setup_param);
        ASSERT_NE(meta_setup_param, nullptr);

        EXPECT_EQ(meta_setup_param->name, EDITED_NAME);
        EXPECT_EQ(meta_setup_param->active_self, EDITED_ACTIVE_SELF);
        EXPECT_EQ(meta_setup_param->tag, EDITED_TAG);
        EXPECT_EQ(meta_setup_param->layer, EDITED_LAYER);
    }

    // Verify edited transform component setup param fields
    {
        ecs::Component::SetupParam* setup_param = setup_param_manager->GetSetupParam(
            test_entity, component_editor_test::TestTransformComponentHandle::ID());
        ASSERT_NE(setup_param, nullptr);

        component_editor_test::TestTransformComponent::SetupParam* transform_setup_param = 
            dynamic_cast<component_editor_test::TestTransformComponent::SetupParam*>(setup_param);
        ASSERT_NE(transform_setup_param, nullptr);

        EXPECT_FLOAT_EQ(transform_setup_param->position.x, EDITED_POSITION.x);
        EXPECT_FLOAT_EQ(transform_setup_param->position.y, EDITED_POSITION.y);
        EXPECT_FLOAT_EQ(transform_setup_param->position.z, EDITED_POSITION.z);

        EXPECT_FLOAT_EQ(transform_setup_param->rotation.x, EDITED_ROTATION.x);
        EXPECT_FLOAT_EQ(transform_setup_param->rotation.y, EDITED_ROTATION.y);
        EXPECT_FLOAT_EQ(transform_setup_param->rotation.z, EDITED_ROTATION.z);

        EXPECT_FLOAT_EQ(transform_setup_param->scale.x, EDITED_SCALE.x);
        EXPECT_FLOAT_EQ(transform_setup_param->scale.y, EDITED_SCALE.y);
        EXPECT_FLOAT_EQ(transform_setup_param->scale.z, EDITED_SCALE.z);
    }
}

TEST(SetupParam, FieldTypeRegistry)
{
    // Access the global setup param field type registry
    component_editor::SetupParamFieldTypeRegistry<int>& registry 
        = component_editor_test::g_setup_param_field_type_registry;

    // String field type
    {
        std::string test_value = "TestString";

        // Get create function for string field type
        const auto& create_func = registry.GetSetupParamFieldCreateFunc("std::string");
        ASSERT_NE(create_func, nullptr);

        // Create std::any object from test value
		std::any any_value = create_func(reinterpret_cast<const uint8_t*>(&test_value), 0);
        ASSERT_EQ(std::any_cast<std::string>(any_value), test_value);

        // Get edit function for string field type
        const auto& edit_func = registry.GetSetupParamFieldEditFunc("std::string");
        ASSERT_NE(edit_func, nullptr);

        // Edit the std::any object
        if (edit_func(any_value, "test_string_field", 0))
        {
            std::string edited_value = std::any_cast<std::string>(any_value);
            std::cout << "Edited string value: " << edited_value << std::endl;
        }

        // Get export function for string field type
        const auto& export_func = registry.GetSetupParamFieldExportFunc("std::string");
        ASSERT_NE(export_func, nullptr);

        // Export the std::any object to JSON
        nlohmann::json field_json = export_func(any_value, "test_string_field", 0);
        nlohmann::json json;
        json["test_string_field"] = field_json;
        std::cout << "Exported JSON: " << json.dump() << std::endl;

        // Get import function for string field type
        const auto& import_func = registry.GetSetupParamFieldImportFunc("std::string");
        ASSERT_NE(import_func, nullptr);

        // Import the std::any object from JSON
        std::any imported_value = import_func("test_string_field", json, 0);
        ASSERT_EQ(std::any_cast<std::string>(imported_value), std::any_cast<std::string>(any_value));
    }

    // Bool field type
    {
        bool test_value = true;
        
        // Get create function for bool field type
        const auto& create_func = registry.GetSetupParamFieldCreateFunc("bool");
        ASSERT_NE(create_func, nullptr);

        // Create std::any object from test value
		std::any any_value = create_func(reinterpret_cast<const uint8_t*>(&test_value), 0);
        ASSERT_EQ(std::any_cast<bool>(any_value), test_value);

        // Get edit function for bool field type
        const auto& edit_func = registry.GetSetupParamFieldEditFunc("bool");
        ASSERT_NE(edit_func, nullptr);

        // Edit the std::any object
        if (edit_func(any_value, "test_bool_field", 0))
        {
            bool edited_value = std::any_cast<bool>(any_value);
            std::cout << "Edited bool value: " << edited_value << std::endl;
        }

        // Get export function for bool field type
        const auto& export_func = registry.GetSetupParamFieldExportFunc("bool");
        ASSERT_NE(export_func, nullptr);

        // Export the std::any object to JSON
        nlohmann::json field_json = export_func(any_value, "test_bool_field", 0);
        nlohmann::json json;
        json["test_bool_field"] = field_json;
        std::cout << "Exported JSON: " << json.dump() << std::endl;

        // Get import function for bool field type
        const auto& import_func = registry.GetSetupParamFieldImportFunc("bool");
        ASSERT_NE(import_func, nullptr);

        // Import the std::any object from JSON
        std::any imported_value = import_func("test_bool_field", json, 0);
        ASSERT_EQ(std::any_cast<bool>(imported_value), std::any_cast<bool>(any_value));
    }
}