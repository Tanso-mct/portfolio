#include "component_editor/src/pch.h"
#include "component_editor/include/setup_param_editor.h"

namespace component_editor
{

bool SetupParamFieldValueSetter::SetFieldValue(
    ecs::Component::SetupParam* setup_param,
    std::string_view field_type, size_t field_offset, std::any new_value)
{
    // Find the set function for the field type
    auto it = set_func_map_.find(std::string(field_type));
    if (it == set_func_map_.end())
        return false; // Set function not found

    // Call the set function
    SetFunc set_func = it->second;
    set_func(setup_param, field_offset, new_value);
    return true; // Successfully set the field value
}

bool SetupParamEditor::IsEditable(ecs::Entity entity, ecs::ComponentID component_id) const
{
    // Check if the setup parameter exists
    bool contains_setup_param =  setup_param_manager_.ContainSetupParam(entity, component_id);
    if (!contains_setup_param)
        return false; // Not editable if setup param does not exist

    // Check if component ID exists in component name map
    bool contains_component_name = (component_name_map_.find(component_id) != component_name_map_.end());
    if (!contains_component_name)
        return false; // Not editable if component name not found

    // Find the component reflection info
    auto it = reflection_registry_.find(ComponentReflectionInfo(component_name_map_.at(component_id), {}));
    bool has_reflection_info = (it != reflection_registry_.end());
    if (!has_reflection_info)
        return false; // Not editable if reflection info not found

    return true; // Editable
}

void SetupParamEditor::Edit(
    ecs::Entity entity, ecs::ComponentID component_id, std::string_view field_name, std::any new_value)
{
    // Get the setup parameter
    ecs::Component::SetupParam* setup_param =
        setup_param_manager_.GetSetupParam(entity, component_id);

    // Get the component reflection info
    auto it = reflection_registry_.find(ComponentReflectionInfo(component_name_map_.at(component_id), {}));
    assert(it != reflection_registry_.end() && "Component reflection info not found.");
    const ComponentReflectionInfo& reflection_info = *it;

    // Get the field map
    const FieldMap& field_map = reflection_info.GetFieldMap();

    // Get the field info
    auto field_it = field_map.find(std::string(field_name));
    assert(field_it != field_map.end() && "Field info not found.");
    const FieldInfo& field_info = field_it->second;

    // Use the field value setter to set the new value
    bool success = field_value_setter_.SetFieldValue(
        setup_param, field_info.type_name, field_info.offset, new_value);
    assert(success && "Failed to set field value.");

    // Record the edited info
    EditedInfo edited_info;
    edited_info.entity = entity;
    edited_info.component_id = component_id;
    edited_infos_.push_back(edited_info);
}

} // namespace component_editor