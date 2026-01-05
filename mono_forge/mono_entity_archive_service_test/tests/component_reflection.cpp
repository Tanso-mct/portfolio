#include "component_editor_test/pch.h"
#include "mono_entity_archive_service_test/tests/component_reflection.h"

using namespace DirectX;

#include "mono_entity_archive_service_test/tests/test_meta_component.h"
#include "mono_entity_archive_service_test/tests/test_transform_component.h"

namespace component_editor_test
{

constexpr const char* TYPE_BOOL = "bool";
constexpr const char* TYPE_STRING = "std::string";
constexpr const char* TYPE_UINT64 = "uint64_t";
constexpr const char* TYPE_XMFLOAT3 = "DirectX::XMFLOAT3";

component_editor::ComponentReflectionRegistry g_component_reflection_registry;

component_editor::ComponentReflectionRegistrar g_test_meta_component_reflection_registrar(
        g_component_reflection_registry, "component_editor_test::TestMetaComponent",
        [](component_editor::ComponentReflectionFieldBuilder& builder)
        {
            builder.ReflectionField(
                "name", "std::string",
                offsetof(component_editor_test::TestMetaComponent::SetupParam, name));
            builder.ReflectionField(
                "active_self", "bool",
                offsetof(component_editor_test::TestMetaComponent::SetupParam, active_self));
            builder.ReflectionField(
                "tag", "uint64_t",
                offsetof(component_editor_test::TestMetaComponent::SetupParam, tag));
            builder.ReflectionField(
                "layer", "uint64_t",
                offsetof(component_editor_test::TestMetaComponent::SetupParam, layer));
        });

component_editor::ComponentReflectionRegistrar g_test_transform_component_reflection_registrar(
        g_component_reflection_registry, "component_editor_test::TestTransformComponent",
        [](component_editor::ComponentReflectionFieldBuilder& builder)
        {
            builder.ReflectionField(
                "position", "DirectX::XMFLOAT3",
                offsetof(component_editor_test::TestTransformComponent::SetupParam, position));
            builder.ReflectionField(
                "rotation", "DirectX::XMFLOAT3",
                offsetof(component_editor_test::TestTransformComponent::SetupParam, rotation));
            builder.ReflectionField(
                "scale", "DirectX::XMFLOAT3",
                offsetof(component_editor_test::TestTransformComponent::SetupParam, scale));
        });

mono_entity_archive_service::ComponentSetupParamFieldTypeRegistry g_setup_param_field_type_registry;

mono_entity_archive_service::ComponentSetupParamFieldTypeRegistrar g_bool_field_type_registrar(
    g_setup_param_field_type_registry,
    TYPE_BOOL,
    [](const uint8_t* field_value, mono_service::ServiceProxyManager& service_proxy_manager) -> std::any
    {
        bool value = *reinterpret_cast<const bool*>(field_value);
        return value;
    },
    [](std::any& value, std::string_view field_name, mono_service::ServiceProxyManager& service_proxy_manager) -> bool
    {
        bool& casted_value = std::any_cast<bool&>(value);
        std::cin >> std::boolalpha >> casted_value;
        return true; // Edited
    },
    [](const std::any& field_value, std::string_view field_name, mono_service::ServiceProxyManager& service_proxy_manager) -> nlohmann::json
    {
        return std::any_cast<const bool&>(field_value);
    },
    [](std::string_view field_name, const nlohmann::json& json, mono_service::ServiceProxyManager& service_proxy_manager) -> std::any
    {
        bool value = json.get<bool>();
        return value;
    });

mono_entity_archive_service::ComponentSetupParamFieldTypeRegistrar g_string_field_type_registrar(
    g_setup_param_field_type_registry,
    TYPE_STRING,
    [](const uint8_t* field_value, mono_service::ServiceProxyManager& service_proxy_manager) -> std::any
    {
        std::string value = *reinterpret_cast<const std::string*>(field_value);
        return value;
    },
    [](std::any& value, std::string_view field_name, mono_service::ServiceProxyManager& service_proxy_manager) -> bool
    {
        std::string new_value;
        std::cout << "Field: " << field_name << " Enter new string value: ";
        std::getline(std::cin, new_value);
        value = new_value;
        return true; // Edited
    },
    [](const std::any& field_value, std::string_view field_name, mono_service::ServiceProxyManager& service_proxy_manager) -> nlohmann::json
    {
        return std::any_cast<const std::string&>(field_value);
    },
    [](std::string_view field_name, const nlohmann::json& json, mono_service::ServiceProxyManager& service_proxy_manager) -> std::any
    {
        std::string value = json.get<std::string>();
        return value;
    });

mono_entity_archive_service::ComponentSetupParamFieldTypeRegistrar g_uint64_field_type_registrar(
    g_setup_param_field_type_registry,
    TYPE_UINT64,
    [](const uint8_t* field_value, mono_service::ServiceProxyManager& service_proxy_manager) -> std::any
    {
        uint64_t value = *reinterpret_cast<const uint64_t*>(field_value);
        return value;
    },
    [](std::any& value, std::string_view field_name, mono_service::ServiceProxyManager& service_proxy_manager) -> bool
    {
        uint64_t& casted_value = std::any_cast<uint64_t&>(value);
        std::cin >> casted_value;
        return true; // Edited
    },
    [](const std::any& field_value, std::string_view field_name, mono_service::ServiceProxyManager& service_proxy_manager) -> nlohmann::json
    {
        return std::any_cast<const uint64_t&>(field_value);
    },
    [](std::string_view field_name, const nlohmann::json& json, mono_service::ServiceProxyManager& service_proxy_manager) -> std::any
    {
        uint64_t value = json.get<uint64_t>();
        return value;
    });

mono_entity_archive_service::ComponentSetupParamFieldTypeRegistrar g_xmfloat3_field_type_registrar(
    g_setup_param_field_type_registry,
    TYPE_XMFLOAT3,
    [](const uint8_t* field_value, mono_service::ServiceProxyManager& service_proxy_manager) -> std::any
    {
        DirectX::XMFLOAT3 value = *reinterpret_cast<const DirectX::XMFLOAT3*>(field_value);
        return value;
    },
    [](std::any& value, std::string_view field_name, mono_service::ServiceProxyManager& service_proxy_manager) -> bool
    {
        DirectX::XMFLOAT3& casted_value = std::any_cast<DirectX::XMFLOAT3&>(value);
        std::cout << "Field: " << field_name << " Enter new XMFLOAT3 values (x y z): ";
        std::cin >> casted_value.x >> casted_value.y >> casted_value.z;
        return true; // Edited
    },
    [](const std::any& field_value, std::string_view field_name, mono_service::ServiceProxyManager& service_proxy_manager) -> nlohmann::json
    {
        DirectX::XMFLOAT3 const& value = std::any_cast<const DirectX::XMFLOAT3&>(field_value);
        nlohmann::json json = nlohmann::json::array();
        json.push_back(value.x);
        json.push_back(value.y);
        json.push_back(value.z);
        return json;
    },
    [](std::string_view field_name, const nlohmann::json& json, mono_service::ServiceProxyManager& service_proxy_manager) -> std::any
    {
        DirectX::XMFLOAT3 value;
        value.x = json[0].get<float>();
        value.y = json[1].get<float>();
        value.z = json[2].get<float>();
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