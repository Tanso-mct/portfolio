#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

namespace component_editor
{

// Information about a field in a component's setup parameter
struct FieldInfo
{
    // Type name of the field
    std::string type_name;

    // Offset of the field within the setup parameter struct
    size_t offset;
};

// The type alias for field map
using FieldMap = std::unordered_map<std::string, FieldInfo>;

// Reflection information for a component
class ComponentReflectionInfo
{
public:
    ComponentReflectionInfo(std::string name, FieldMap field_map) :
        component_name_(std::move(name)),
        field_map_(std::move(field_map))
    {
    }

    ~ComponentReflectionInfo() = default;

    bool operator==(const ComponentReflectionInfo& other) const
    {
        // Compare based on component name
        return component_name_ == other.component_name_;
    }

    // Get the component name
    std::string_view GetComponentName() const { return component_name_; }

    // Get the field map
    const FieldMap& GetFieldMap() const { return field_map_; }

private:
    // Name of the component including namespace
    std::string component_name_;

    // Map of field name to FieldInfo
    FieldMap field_map_;
};

} // namespace component_editor

namespace std
{
    template<>
    struct hash<component_editor::ComponentReflectionInfo>
    {
        size_t operator()(const component_editor::ComponentReflectionInfo& info) const
        {
            return hash<std::string>()(std::string(info.GetComponentName()));
        }
    };

} // namespace std