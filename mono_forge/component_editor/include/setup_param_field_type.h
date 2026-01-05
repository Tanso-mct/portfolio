#pragma once

#include "component_editor/include/dll_config.h"
#include "component_editor/include/json.hpp"

#include <functional>
#include <any>
#include <string>
#include <string_view>
#include <unordered_map>
#include <cassert>

namespace component_editor
{

// Type alias for setup param field create function
// Returns a std::any object representing the field value
template <typename ...Args>
using SetupParamAnyFieldCreateFunc = std::function<std::any(const uint8_t* field_value, Args... args)>;

// Type alias for setup param field edit function
// If returned bool is true, the field was edited
template <typename ...Args>
using SetupParamAnyFieldEditFunc = std::function<bool(std::any& field_value, std::string_view field_name, Args... args)>;

// Type alias for setup param field export function
// Returns a nlohmann::json object representing the field value
template <typename ...Args>
using SetupParamAnyFieldExportFunc 
    = std::function<nlohmann::json(const std::any& field_value, std::string_view field_name, Args... args)>;

// Type alias for setup param field import function
// Modifies the field_value std::any object based on the provided nlohmann::json object
template <typename ...Args>
using SetupParamAnyFieldImportFunc 
    = std::function<std::any(std::string_view field_name, const nlohmann::json& json, Args... args)>;

template <typename... Args>
class SetupParamFieldTypeRegistry
{
public:
    SetupParamFieldTypeRegistry() = default;
    ~SetupParamFieldTypeRegistry() = default;

    // Register setup param field type
    void RegisterSetupParamFieldType(
        std::string type_name,
        SetupParamAnyFieldCreateFunc<Args...> create_func,
        SetupParamAnyFieldEditFunc<Args...> edit_func,
        SetupParamAnyFieldExportFunc<Args...> export_func,
        SetupParamAnyFieldImportFunc<Args...> import_func)
    {
        
        // Register create function
        assert(
            type_name_to_create_func_map_.find(type_name) == type_name_to_create_func_map_.end()
            && "Setup param field type already registered.");
        type_name_to_create_func_map_[type_name] = std::move(create_func);

        // Register edit function
        assert(
            type_name_to_edit_func_map_.find(type_name) == type_name_to_edit_func_map_.end()
            && "Setup param field type already registered.");
        type_name_to_edit_func_map_[type_name] = std::move(edit_func);

        // Register export function
        assert(
            type_name_to_export_func_map_.find(type_name) == type_name_to_export_func_map_.end()
            && "Setup param field type already registered.");
        type_name_to_export_func_map_[type_name] = std::move(export_func);

        // Register import function
        assert(
            type_name_to_import_func_map_.find(type_name) == type_name_to_import_func_map_.end()
            && "Setup param field type already registered.");
        type_name_to_import_func_map_[type_name] = std::move(import_func);
    }

    // Get setup param field create function
    const SetupParamAnyFieldCreateFunc<Args...>& GetSetupParamFieldCreateFunc(std::string_view type_name) const
    {
        auto it = type_name_to_create_func_map_.find(type_name.data());
        assert(it != type_name_to_create_func_map_.end() && "Setup param field type not found.");
        return it->second;
    }

    // Get setup param field edit function
    const SetupParamAnyFieldEditFunc<Args...>& GetSetupParamFieldEditFunc(std::string_view type_name) const
    {
        auto it = type_name_to_edit_func_map_.find(type_name.data());
        assert(it != type_name_to_edit_func_map_.end() && "Setup param field type not found.");
        return it->second;
    }

    // Get setup param field export function
    const SetupParamAnyFieldExportFunc<Args...>& GetSetupParamFieldExportFunc(std::string_view type_name) const
    {
        auto it = type_name_to_export_func_map_.find(type_name.data());
        assert(it != type_name_to_export_func_map_.end() && "Setup param field type not found.");
        return it->second;
    }

    // Get setup param field import function
    const SetupParamAnyFieldImportFunc<Args...>& GetSetupParamFieldImportFunc(std::string_view type_name) const
    {
        auto it = type_name_to_import_func_map_.find(type_name.data());
        assert(it != type_name_to_import_func_map_.end() && "Setup param field type not found.");
        return it->second;
    }

private:
    // Map from type name to setup param field create function
    std::unordered_map<std::string, SetupParamAnyFieldCreateFunc<Args...>> type_name_to_create_func_map_;

    // Map from type name to setup param field edit function
    std::unordered_map<std::string, SetupParamAnyFieldEditFunc<Args...>> type_name_to_edit_func_map_;

    // Map from type name to setup param field export function
    std::unordered_map<std::string, SetupParamAnyFieldExportFunc<Args...>> type_name_to_export_func_map_;

    // Map from type name to setup param field import function
    std::unordered_map<std::string, SetupParamAnyFieldImportFunc<Args...>> type_name_to_import_func_map_;
};

// Registrar for setup param field types
template <typename... Args>
class SetupParamFieldTypeRegistrar
{
public:
    SetupParamFieldTypeRegistrar(
        SetupParamFieldTypeRegistry<Args...>& registry,
        std::string type_name,
        SetupParamAnyFieldCreateFunc<Args...> create_func,
        SetupParamAnyFieldEditFunc<Args...> edit_func,
        SetupParamAnyFieldExportFunc<Args...> export_func,
        SetupParamAnyFieldImportFunc<Args...> import_func)
    {
        registry.RegisterSetupParamFieldType(
            std::move(type_name), 
            std::move(create_func), std::move(edit_func), std::move(export_func), std::move(import_func));
    }

    ~SetupParamFieldTypeRegistrar() = default;
};

} // namespace component_editor