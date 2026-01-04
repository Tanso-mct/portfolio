#pragma once

#include <functional>

#include "component_editor/include/dll_config.h"
#include "component_editor/include/component_reflection_registry.h"
#include "component_editor/include/component_reflection_info.h"

namespace component_editor
{

// Builder class to help construct field maps for component reflection
class COMPONENT_EDITOR_DLL ComponentReflectionFieldBuilder
{
public:
    ComponentReflectionFieldBuilder(FieldMap& field_map) :
        field_map_(field_map)
    {
    }

    ~ComponentReflectionFieldBuilder() = default;

    // Add a field to the field map
    void ReflectionField(std::string name, std::string type, size_t offset);

private:
    // Reference to the field map being built
    FieldMap& field_map_;
};

// Registrar class to register component reflection information
class COMPONENT_EDITOR_DLL ComponentReflectionRegistrar
{
public:
    // Type alias for the reflection registration function
    using ReflectionRegisterFunc = std::function<void(ComponentReflectionFieldBuilder&)>;

    ComponentReflectionRegistrar(
        ComponentReflectionRegistry& registry, std::string component_name, ReflectionRegisterFunc register_func);
    ~ComponentReflectionRegistrar() = default;
};

} // namespace component_editor