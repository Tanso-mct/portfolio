#include "component_editor/src/pch.h"
#include "component_editor/include/component_reflection_registrar.h"

#include "utility_header/logger.h"

namespace component_editor
{

void ComponentReflectionFieldBuilder::ReflectionField(std::string name, std::string type, size_t offset)
{
    // Create FieldInfo
    FieldInfo field_info;
    field_info.type_name = std::move(type);
    field_info.offset = offset;

    assert(field_map_.find(name) == field_map_.end() && "Field already exists in the field map!");

    // Insert the field info into the field map
    field_map_[std::move(name)] = std::move(field_info);
}

ComponentReflectionRegistrar::ComponentReflectionRegistrar(
    ComponentReflectionRegistry& registry, std::string component_name, ReflectionRegisterFunc register_func)
{
    // Create FieldMap to hold fields
    FieldMap field_map;

    // Create ComponentReflectionFieldBuilder
    ComponentReflectionFieldBuilder builder(field_map);

    // Call the registration function to populate the field map
    register_func(builder);

    // Create ComponentReflectionInfo
    ComponentReflectionInfo reflection_info(std::move(component_name), std::move(field_map));

    // Insert into the registry
    assert(
        registry.find(reflection_info) == registry.end() && 
        "ComponentReflectionInfo already exists in the registry!");
    registry.insert(std::move(reflection_info));
}

} // namespace component_editor