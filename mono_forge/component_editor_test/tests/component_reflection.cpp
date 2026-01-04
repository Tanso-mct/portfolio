#include "component_editor_test/pch.h"
#include "component_editor_test/tests/component_reflection.h"

#include "component_editor_test/tests/test_meta_component.h"
#include "component_editor_test/tests/test_transform_component.h"

namespace component_editor_test
{

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

} // namespace component_editor_test