#pragma once

#include <string_view>
#include <any>
#include <memory>
#include <functional>

#include "ecs/include/world.h"
#include "component_editor/include/dll_config.h"
#include "component_editor/include/component_reflection_registry.h"
#include "component_editor/include/setup_param_manager.h"

namespace component_editor
{

// Class to set field values in setup parameters
class COMPONENT_EDITOR_DLL SetupParamFieldValueSetter
{
public:
    SetupParamFieldValueSetter() = default;
    virtual ~SetupParamFieldValueSetter() = default;

    // Cast the field value to the appropriate type and set it in the setup parameter
    bool SetFieldValue(
        ecs::Component::SetupParam* setup_param,
        std::string_view field_type, size_t field_offset, std::any new_value) const;

    // Register a field type with its corresponding set function
    template <typename T>
    void RegisterFieldTypeSetFunc(std::string_view field_type)
    {
        set_func_map_[field_type.data()] = [this](
            ecs::Component::SetupParam* setup_param, size_t field_offset, std::any new_value)
            {
                // Cast the field pointer to the appropriate type
                T* field_type_ptr
                    = reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(setup_param) + field_offset);

                // Cast the new value to the appropriate type
                T new_value_type_ptr = std::any_cast<T>(new_value);

                // Set the field value
                *field_type_ptr = new_value_type_ptr;
            };
    }

    // Registrar for automatic registration of field types
    template <typename T>
    class Registrar
    {
    public:
        Registrar(SetupParamFieldValueSetter& field_value_setter, std::string_view field_type)
        {
            field_value_setter.RegisterFieldTypeSetFunc<T>(field_type);
        }
    };

private:
    // Function type for setting field values
    using SetFunc 
        = std::function<void(ecs::Component::SetupParam* setup_param, size_t field_offset, std::any new_value)>;

    // Map of field type to set function
    std::unordered_map<std::string, SetFunc> set_func_map_;
};

// Structure to hold information about edited setup parameters
struct EditedInfo
{
    ecs::Entity entity;
    ecs::ComponentID component_id;
};

// Editor for component setup parameters
class COMPONENT_EDITOR_DLL SetupParamEditor
{
public:
    SetupParamEditor(
        SetupParamManager& setup_param_manager, 
        const ComponentNameMap& component_name_map,
        const ComponentReflectionRegistry& reflection_registry,
        SetupParamFieldValueSetter field_value_setter) :
        setup_param_manager_(setup_param_manager),
        component_name_map_(component_name_map),
        reflection_registry_(reflection_registry),
        field_value_setter_(std::move(field_value_setter))
    {
    }

    virtual ~SetupParamEditor() = default;

    // Check if the component's setup parameters are editable
    bool IsEditable(ecs::Entity entity, ecs::ComponentID component_id) const;

    // Edit the component's setup parameters
    void Edit(
        ecs::Entity entity, ecs::ComponentID component_id, std::string_view field_name, std::any new_value);

    // Get the list of edited setup parameters
    const std::vector<EditedInfo>& GetEditedInfos() const { return edited_infos_; }

    // Clear the list of edited setup parameters
    void ClearEditedInfos() { edited_infos_.clear(); }

    // Set a field value in the setup parameter
    bool SetFieldValue(
        ecs::Component::SetupParam* setup_param,
        std::string_view field_type, size_t field_offset, std::any new_value) const;

private:
    // The reference to the setup parameter manager
    SetupParamManager& setup_param_manager_;
    
    // The reference to the component name map
    const ComponentNameMap& component_name_map_;

    // The reference to the component reflection registry
    const ComponentReflectionRegistry& reflection_registry_;

    // The field value setter
    SetupParamFieldValueSetter field_value_setter_;

    // The list of edited setup parameters
    std::vector<EditedInfo> edited_infos_;
};

} // namespace component_editor