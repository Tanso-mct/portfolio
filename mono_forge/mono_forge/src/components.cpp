#include "mono_forge/src/pch.h"
#include "mono_forge/include/components.h"

#include "utility_header/logger.h"

#include "component_editor/include/component_reflection_registrar.h"
#include "component_editor/include/component_name_registrar.h"
#include "component_editor/include/component_adder_registrar.h"

//COMPONENTS_INCLUDE_BEGIN//
#include "mono_meta_extension/include/allocator_factory.h"
#include "mono_meta_extension/include/meta_component.h"

#include "mono_asset_service/include/asset_service.h"
#include "mono_asset_service/include/asset_service_view.h"
#include "mono_asset_extension/include/mesh_asset.h"
#include "mono_asset_extension/include/allocator_factory.h"
#include "mono_asset_extension/include/asset_request_component.h"

#include "mono_transform_extension/include/allocator_factory.h"
#include "mono_transform_extension/include/transform_component.h"

#include "mono_scene_extension/include/allocator_factory.h"
#include "mono_scene_extension/include/scene_component.h"
#include "mono_scene_extension/include/scene_tag_component.h"

#include "mono_window_extension/include/allocator_factory.h"
#include "mono_window_extension/include/window_component.h"

#include "render_graph/include/material_handle_manager.h"
#include "mono_graphics_extension/include/allocator_factory.h"
#include "mono_graphics_extension/include/window_render_bind_component.h"
#include "mono_graphics_extension/include/camera_component.h"
#include "mono_graphics_extension/include/renderable_component.h"
#include "mono_graphics_extension/include/ui_component.h"
#include "mono_graphics_extension/include/directional_light_component.h"
#include "mono_graphics_extension/include/ambient_light_component.h"
#include "mono_graphics_extension/include/point_light_component.h"

#include "mono_entity_archive_service/include/entity_archive_service.h"
#include "mono_entity_archive_extension/include/allocator_factory.h"
#include "mono_entity_archive_extension/include/editor_enabled_component.h"
#include "mono_entity_archive_extension/include/hierarchy_ui_component.h"
#include "mono_entity_archive_extension/include/inspector_ui_component.h"
#include "mono_entity_archive_extension/include/asset_browser_ui_component.h"
#include "mono_entity_archive_extension/include/transform_manipulator_ui_component.h"
#include "mono_entity_archive_extension/include/menu_bar_ui_component.h"
#include "mono_entity_archive_extension/include/material_editor_ui_component.h"

//COMPONENTS_INCLUDE_END//

#include "mono_forge/include/component_adders.h"

namespace mono_forge
{

ecs::ComponentIDGenerator g_component_id_generator;
component_editor::ComponentReflectionRegistry g_component_reflection_registry;
mono_entity_archive_service::ComponentSetupParamFieldTypeRegistry g_setup_param_field_type_registry;
component_editor::SetupParamFieldValueSetter g_component_field_value_setter;
component_editor::ComponentNameMap g_component_name_map;
component_editor::ComponentAdderMap g_component_adder_map;
ecs::ComponentDescriptorRegistry g_component_descriptor_registry;

//TYPE_DEFINE_BEGIN//

constexpr const char* TYPE_BOOL = "bool";
constexpr const char* TYPE_INT = "int";
constexpr const char* TYPE_FLOAT = "float";
constexpr const char* TYPE_STD_STRING = "std::string";
constexpr const char* TYPE_UINT64 = "uint64_t";
constexpr const char* TYPE_XMFLOAT3 = "DirectX::XMFLOAT3";
constexpr const char* TYPE_XMFLOAT4 = "DirectX::XMFLOAT4";
constexpr const char* TYPE_XMFLOAT4_COLOR = "DirectX::XMFLOAT4@COLOR";
constexpr const char* TYPE_MESH_SET = "mono_graphics_extension::MeshSet";

//TYPE_DEFINE_END//

//COMPONENT_REFLECTION_REGISTRAR_DEFINE_BEGIN//

component_editor::ComponentReflectionRegistrar g_meta_component_reflection_registrar(
    g_component_reflection_registry, "mono_meta_extension::MetaComponent",
    [](component_editor::ComponentReflectionFieldBuilder& builder)
    {
        builder.ReflectionField(
            "name", TYPE_STD_STRING,
            offsetof(mono_meta_extension::MetaComponent::SetupParam, name));
        builder.ReflectionField(
            "active_self", TYPE_BOOL,
            offsetof(mono_meta_extension::MetaComponent::SetupParam, active_self));
        builder.ReflectionField(
            "tag", TYPE_UINT64,
            offsetof(mono_meta_extension::MetaComponent::SetupParam, tag));
        builder.ReflectionField(
            "layer", TYPE_UINT64,
            offsetof(mono_meta_extension::MetaComponent::SetupParam, layer));
    });

component_editor::ComponentReflectionRegistrar g_transform_component_reflection_registrar(
    g_component_reflection_registry, "mono_transform_extension::TransformComponent",
    [](component_editor::ComponentReflectionFieldBuilder& builder)
    {
        builder.ReflectionField(
            "position", TYPE_XMFLOAT3,
            offsetof(mono_transform_extension::TransformComponent::SetupParam, position));
        builder.ReflectionField(
            "rotation", TYPE_XMFLOAT3,
            offsetof(mono_transform_extension::TransformComponent::SetupParam, rotation));
        builder.ReflectionField(
            "scale", TYPE_XMFLOAT3,
            offsetof(mono_transform_extension::TransformComponent::SetupParam, scale));
    });

component_editor::ComponentReflectionRegistrar g_camera_component_reflection_registrar(
    g_component_reflection_registry, "mono_graphics_extension::CameraComponent",
    [](component_editor::ComponentReflectionFieldBuilder& builder)
    {
        builder.ReflectionField(
            "fov_y", TYPE_FLOAT,
            offsetof(mono_graphics_extension::CameraComponent::SetupParam, fov_y));
        builder.ReflectionField(
            "aspect_ratio", TYPE_FLOAT,
            offsetof(mono_graphics_extension::CameraComponent::SetupParam, aspect_ratio));
        builder.ReflectionField(
            "near_z", TYPE_FLOAT,
            offsetof(mono_graphics_extension::CameraComponent::SetupParam, near_z));
        builder.ReflectionField(
            "far_z", TYPE_FLOAT,
            offsetof(mono_graphics_extension::CameraComponent::SetupParam, far_z));
    });

component_editor::ComponentReflectionRegistrar g_directional_light_component_reflection_registrar(
    g_component_reflection_registry, "mono_graphics_extension::DirectionalLightComponent",
    [](component_editor::ComponentReflectionFieldBuilder& builder)
    {
        builder.ReflectionField(
            "light_color", TYPE_XMFLOAT4_COLOR,
            offsetof(mono_graphics_extension::DirectionalLightComponent::SetupParam, light_color));
        builder.ReflectionField(
            "intensity", TYPE_FLOAT,
            offsetof(mono_graphics_extension::DirectionalLightComponent::SetupParam, intensity));
        builder.ReflectionField(
            "distance", TYPE_FLOAT,
            offsetof(mono_graphics_extension::DirectionalLightComponent::SetupParam, distance));
        builder.ReflectionField(
            "ortho_width", TYPE_FLOAT,
            offsetof(mono_graphics_extension::DirectionalLightComponent::SetupParam, ortho_width));
        builder.ReflectionField(
            "ortho_height", TYPE_FLOAT,
            offsetof(mono_graphics_extension::DirectionalLightComponent::SetupParam, ortho_height));
        builder.ReflectionField(
            "near_z", TYPE_FLOAT,
            offsetof(mono_graphics_extension::DirectionalLightComponent::SetupParam, near_z));
        builder.ReflectionField(
            "far_z", TYPE_FLOAT,
            offsetof(mono_graphics_extension::DirectionalLightComponent::SetupParam, far_z));
    });

component_editor::ComponentReflectionRegistrar g_ambient_light_component_reflection_registrar(
    g_component_reflection_registry, "mono_graphics_extension::AmbientLightComponent",
    [](component_editor::ComponentReflectionFieldBuilder& builder)
    {
        builder.ReflectionField(
            "light_color", TYPE_XMFLOAT4_COLOR,
            offsetof(mono_graphics_extension::AmbientLightComponent::SetupParam, light_color));
        builder.ReflectionField(
            "intensity", TYPE_FLOAT,
            offsetof(mono_graphics_extension::AmbientLightComponent::SetupParam, intensity));
    });

component_editor::ComponentReflectionRegistrar g_point_light_component_reflection_registrar(
    g_component_reflection_registry, "mono_graphics_extension::PointLightComponent",
    [](component_editor::ComponentReflectionFieldBuilder& builder)
    {
        builder.ReflectionField(
            "light_color", TYPE_XMFLOAT4_COLOR,
            offsetof(mono_graphics_extension::PointLightComponent::SetupParam, light_color));
        builder.ReflectionField(
            "intensity", TYPE_FLOAT,
            offsetof(mono_graphics_extension::PointLightComponent::SetupParam, intensity));
        builder.ReflectionField(
            "range", TYPE_FLOAT,
            offsetof(mono_graphics_extension::PointLightComponent::SetupParam, range));
    });

component_editor::ComponentReflectionRegistrar g_renderable_component_reflection_registrar(
    g_component_reflection_registry, "mono_graphics_extension::RenderableComponent",
    [](component_editor::ComponentReflectionFieldBuilder& builder)
    {
        builder.ReflectionField(
            "mesh_set", TYPE_MESH_SET,
            offsetof(mono_graphics_extension::RenderableComponent::SetupParam, mesh_set_));
        builder.ReflectionField(
            "cast_shadows", TYPE_BOOL,
            offsetof(mono_graphics_extension::RenderableComponent::SetupParam, cast_shadow_));
    });

//COMPONENT_REFLECTION_REGISTRAR_DEFINE_END//

//SETUP_PARAM_FIELD_TYPE_REGISTRAR_DEFINE_BEGIN//

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
        if (ImGui::Checkbox(field_name.data(), &casted_value))
            return true; // Edited

        return false; // Not edited
    });

mono_entity_archive_service::ComponentSetupParamFieldTypeRegistrar g_int_field_type_registrar(
    g_setup_param_field_type_registry,
    "int",
    [](const uint8_t* field_value, mono_service::ServiceProxyManager& service_proxy_manager) -> std::any
    {
        int value = *reinterpret_cast<const int*>(field_value);
        return value;
    },
    [](std::any& value, std::string_view field_name, mono_service::ServiceProxyManager& service_proxy_manager) -> bool
    {
        int& casted_value = std::any_cast<int&>(value);
        if (ImGui::DragScalar(field_name.data(), ImGuiDataType_S32, &casted_value, 1.0f))
            return true; // Edited

        return false; // Not edited
    });

mono_entity_archive_service::ComponentSetupParamFieldTypeRegistrar g_float_field_type_registrar(
    g_setup_param_field_type_registry,
    TYPE_FLOAT,
    [](const uint8_t* field_value, mono_service::ServiceProxyManager& service_proxy_manager) -> std::any
    {
        float value = *reinterpret_cast<const float*>(field_value);
        return value;
    },
    [](std::any& value, std::string_view field_name, mono_service::ServiceProxyManager& service_proxy_manager) -> bool
    {
        float& casted_value = std::any_cast<float&>(value);
        if (ImGui::DragScalar(field_name.data(), ImGuiDataType_Float, &casted_value, 0.01f))
            return true; // Edited

        return false; // Not edited
    });

mono_entity_archive_service::ComponentSetupParamFieldTypeRegistrar g_string_field_type_registrar(
    g_setup_param_field_type_registry,
    TYPE_STD_STRING,
    [](const uint8_t* field_value, mono_service::ServiceProxyManager& service_proxy_manager) -> std::any
    {
        std::string value = *reinterpret_cast<const std::string*>(field_value);
        return value;
    },
    [](std::any& value, std::string_view field_name, mono_service::ServiceProxyManager& service_proxy_manager) -> bool
    {
        std::string& casted_value = std::any_cast<std::string&>(value);
        assert(casted_value.size() < 256 && "String value too long for ImGui input buffer.");
        char buffer[256] = {};
        std::strncpy(buffer, casted_value.c_str(), sizeof(buffer));
        if (ImGui::InputText(field_name.data(), buffer, sizeof(buffer)))
        {
            casted_value = std::string(buffer);
            if (casted_value == "")
                casted_value = " "; // Prevent empty string

            return true; // Edited
        }

        return false; // Not edited
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
        if (ImGui::DragScalar(field_name.data(), ImGuiDataType_U64, &value, 0.01f))
            return true; // Edited

        return false; // Not edited
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
        if (ImGui::DragFloat3(field_name.data(), reinterpret_cast<float*>(&casted_value), 0.01f))
            return true; // Edited

        return false; // Not edited
    });

mono_entity_archive_service::ComponentSetupParamFieldTypeRegistrar g_xmfloat4_field_type_registrar(
    g_setup_param_field_type_registry,
    TYPE_XMFLOAT4,
    [](const uint8_t* field_value, mono_service::ServiceProxyManager& service_proxy_manager) -> std::any
    {
        DirectX::XMFLOAT4 value = *reinterpret_cast<const DirectX::XMFLOAT4*>(field_value);
        return value;
    },
    [](std::any& value, std::string_view field_name, mono_service::ServiceProxyManager& service_proxy_manager) -> bool
    {
        DirectX::XMFLOAT4& casted_value = std::any_cast<DirectX::XMFLOAT4&>(value);
        if (ImGui::DragFloat4(field_name.data(), &casted_value.x))
            return true; // Edited

        return false; // Not edited
    });

mono_entity_archive_service::ComponentSetupParamFieldTypeRegistrar g_xmfloat4_color_field_type_registrar(
    g_setup_param_field_type_registry,
    TYPE_XMFLOAT4_COLOR,
    [](const uint8_t* field_value, mono_service::ServiceProxyManager& service_proxy_manager) -> std::any
    {
        DirectX::XMFLOAT4 value = *reinterpret_cast<const DirectX::XMFLOAT4*>(field_value);
        return value;
    },
    [](std::any& value, std::string_view field_name, mono_service::ServiceProxyManager& service_proxy_manager) -> bool
    {
        DirectX::XMFLOAT4& casted_value = std::any_cast<DirectX::XMFLOAT4&>(value);
        if (ImGui::ColorEdit4(field_name.data(), &casted_value.x))
            return true; // Edited

        return false; // Not edited
    });

mono_entity_archive_service::ComponentSetupParamFieldTypeRegistrar g_mesh_set_field_type_registrar(
    g_setup_param_field_type_registry,
    TYPE_MESH_SET,
    [](const uint8_t* field_value, mono_service::ServiceProxyManager& service_proxy_manager) -> std::any
    {
        mono_graphics_extension::MeshSet value = *reinterpret_cast<const mono_graphics_extension::MeshSet*>(field_value);
        return value;
    },
    [](std::any& value, std::string_view field_name, mono_service::ServiceProxyManager& service_proxy_manager) -> bool
    {
        mono_graphics_extension::MeshSet& casted_value = std::any_cast<mono_graphics_extension::MeshSet&>(value);

        // Get asset service view
        std::unique_ptr<mono_service::ServiceView> asset_service_view = nullptr;
        service_proxy_manager.WithLock([&](mono_service::ServiceProxyManager& service_proxy_manager_inner)
        {
            // Get asset service proxy
            mono_service::ServiceProxy& asset_service_proxy 
                = service_proxy_manager_inner.GetServiceProxy(mono_asset_service::AssetServiceHandle::ID());

            // Create asset service view
            asset_service_view = asset_service_proxy.CreateView();
        });

        // Cast to AssetServiceView
        mono_asset_service::AssetServiceView* asset_service_view_ptr
            = dynamic_cast<mono_asset_service::AssetServiceView*>(asset_service_view.get());

        // Get loaded asset IDs
        std::vector<asset_loader::AssetHandleID> loaded_asset_ids = asset_service_view_ptr->GetLoadedAssetIDs();

        // Create names
        std::string custom_mesh_field_name = std::string(field_name) + " (Click to select mesh asset)";
        std::string custom_material_field_name = std::string(field_name) + " (Click to select material)";
        std::string mesh_popup_name = std::string(field_name) + "mesh_popup";
        std::string material_popup_name = std::string(field_name) + "material_popup";

        // Display popup text
        ImGui::Text("%s", custom_mesh_field_name.data());
        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            ImGui::OpenPopup(mesh_popup_name.data());

        // Popup for mesh asset selection
        if (ImGui::BeginPopup(mesh_popup_name.data()))
        {
            for (const asset_loader::AssetHandleID& asset_id : loaded_asset_ids)
            {
                // Get asset
                const asset_loader::Asset& asset = asset_service_view_ptr->GetAsset(asset_id);

                // Try to cast to MeshAsset
                const mono_asset_extension::MeshAsset* mesh_asset
                    = dynamic_cast<const mono_asset_extension::MeshAsset*>(&asset);

                // If cast succeeded
                if (mesh_asset != nullptr)
                {
                    // Display selectable for each mesh asset
                    std::string element_name = std::string(mesh_asset->GetName()) + "##" + std::to_string(asset_id);
                    if (ImGui::Selectable(element_name.c_str()))
                    {
                        // On selection, set the mesh handles
                        casted_value.vertex_buffer_handles = *mesh_asset->GetVertexBufferHandles();
                        casted_value.index_buffer_handles = *mesh_asset->GetIndexBufferHandles();
                        casted_value.index_counts = *mesh_asset->GetIndexCounts();

                        ImGui::EndPopup();
                        return true; // Edited
                    }
                }
            }
            
            ImGui::EndPopup();
        }

        // Display popup text for material selection
        ImGui::Text("%s", custom_material_field_name.data());
        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            ImGui::OpenPopup(material_popup_name.data());

        // Popup for material selection
        if (ImGui::BeginPopup(material_popup_name.data()))
        {
            // Get all registered material handle IDs
            std::vector<uint32_t> registered_material_handle_ids
                = render_graph::MaterialHandleManager::GetInstance().GetRegisteredMaterialHandleKeys();

            for (const uint32_t& material_handle_id : registered_material_handle_ids)
            {
                // Get material name
                std::string_view material_name
                    = render_graph::MaterialHandleManager::GetInstance().GetMaterialHandleName(material_handle_id);

                // Display selectable for each material
                std::string element_name = std::string(material_name) + "##" + std::to_string(material_handle_id);
                if (ImGui::Selectable(element_name.c_str()))
                {
                    // On selection, set the material handle
                    for (size_t i = 0; i < casted_value.material_handles.size(); ++i)
                    {
                        casted_value.material_handles[i]
                            = render_graph::MaterialHandleManager::GetInstance().GetMaterialHandle(material_handle_id);
                    }

                    ImGui::EndPopup();
                    return true; // Edited
                }
            }
            
            ImGui::EndPopup();
        }

        return false; // Not edited
    });

//SETUP_PARAM_FIELD_TYPE_REGISTRAR_DEFINE_END//

//SETUP_PARAM_FIELD_VALUE_SETTER_DEFINE_BEGIN//

component_editor::SetupParamFieldValueSetter::Registrar<bool> g_bool_field_value_setter_registrar(
    g_component_field_value_setter, TYPE_BOOL);

component_editor::SetupParamFieldValueSetter::Registrar<int> g_int_field_value_setter_registrar(
    g_component_field_value_setter, TYPE_INT);

component_editor::SetupParamFieldValueSetter::Registrar<float> g_float_field_value_setter_registrar(
    g_component_field_value_setter, TYPE_FLOAT);

component_editor::SetupParamFieldValueSetter::Registrar<std::string> g_string_field_value_setter_registrar(
    g_component_field_value_setter, TYPE_STD_STRING);

component_editor::SetupParamFieldValueSetter::Registrar<uint64_t> g_uint64_field_value_setter_registrar(
    g_component_field_value_setter, TYPE_UINT64);

component_editor::SetupParamFieldValueSetter::Registrar<DirectX::XMFLOAT3> g_xmfloat3_field_value_setter_registrar(
    g_component_field_value_setter, TYPE_XMFLOAT3);

component_editor::SetupParamFieldValueSetter::Registrar<DirectX::XMFLOAT4> g_xmfloat4_field_value_setter_registrar(
    g_component_field_value_setter, TYPE_XMFLOAT4);

component_editor::SetupParamFieldValueSetter::Registrar<DirectX::XMFLOAT4> g_xmfloat4_color_field_value_setter_registrar(
    g_component_field_value_setter, TYPE_XMFLOAT4_COLOR);

component_editor::SetupParamFieldValueSetter::Registrar<mono_graphics_extension::MeshSet> g_mesh_set_field_value_setter_registrar(
    g_component_field_value_setter, TYPE_MESH_SET);

//COMPONENT_FIELD_VALUE_SETTER_DEFINE_END//

//COMPONENT_NAME_REGISTRAR_DEFINE_BEGIN//

component_editor::ComponentNameRegistrar g_meta_component_name_registrar(
    g_component_name_map,
    mono_meta_extension::MetaComponentHandle::ID(),
    "mono_meta_extension::MetaComponent");

component_editor::ComponentNameRegistrar g_transform_component_name_registrar(
    g_component_name_map,
    mono_transform_extension::TransformComponentHandle::ID(),
    "mono_transform_extension::TransformComponent");

component_editor::ComponentNameRegistrar g_camera_component_name_registrar(
    g_component_name_map,
    mono_graphics_extension::CameraComponentHandle::ID(),
    "mono_graphics_extension::CameraComponent");

component_editor::ComponentNameRegistrar g_directional_light_component_name_registrar(
    g_component_name_map,
    mono_graphics_extension::DirectionalLightComponentHandle::ID(),
    "mono_graphics_extension::DirectionalLightComponent");

component_editor::ComponentNameRegistrar g_ambient_light_component_name_registrar(
    g_component_name_map,
    mono_graphics_extension::AmbientLightComponentHandle::ID(),
    "mono_graphics_extension::AmbientLightComponent");

component_editor::ComponentNameRegistrar g_point_light_component_name_registrar(
    g_component_name_map,
    mono_graphics_extension::PointLightComponentHandle::ID(),
    "mono_graphics_extension::PointLightComponent");

component_editor::ComponentNameRegistrar g_renderable_component_name_registrar(
    g_component_name_map,
    mono_graphics_extension::RenderableComponentHandle::ID(),
    "mono_graphics_extension::RenderableComponent");

//SETUP_PARAM_FIELD_VALUE_SETTER_DEFINE_END//

//COMPONENT_ADDER_REGISTRAR_DEFINE_BEGIN//

component_editor::ComponentAdderRegistrar g_meta_component_adder_registrar(
    g_component_adder_map,
    mono_meta_extension::MetaComponentHandle::ID(),
    std::make_unique<MetaComponentAdder>());

component_editor::ComponentAdderRegistrar g_transform_component_adder_registrar(
    g_component_adder_map,
    mono_transform_extension::TransformComponentHandle::ID(),
    std::make_unique<TransformComponentAdder>());

component_editor::ComponentAdderRegistrar g_camera_component_adder_registrar(
    g_component_adder_map,
    mono_graphics_extension::CameraComponentHandle::ID(),
    std::make_unique<CameraComponentAdder>());

component_editor::ComponentAdderRegistrar g_directional_light_component_adder_registrar(
    g_component_adder_map,
    mono_graphics_extension::DirectionalLightComponentHandle::ID(),
    std::make_unique<DirectionalLightComponentAdder>());

component_editor::ComponentAdderRegistrar g_ambient_light_component_adder_registrar(
    g_component_adder_map,
    mono_graphics_extension::AmbientLightComponentHandle::ID(),
    std::make_unique<AmbientLightComponentAdder>());

component_editor::ComponentAdderRegistrar g_point_light_component_adder_registrar(
    g_component_adder_map,
    mono_graphics_extension::PointLightComponentHandle::ID(),
    std::make_unique<PointLightComponentAdder>());

component_editor::ComponentAdderRegistrar g_renderable_component_adder_registrar(
    g_component_adder_map,
    mono_graphics_extension::RenderableComponentHandle::ID(),
    std::make_unique<RenderableComponentAdder>());

//COMPONENT_ADDER_REGISTRAR_DEFINE_END//

//COMPONENT_DESCRIPTOR_REGISTRAR_DEFINE_BEGIN//

constexpr size_t MAX_META_COMPONENT_COUNT = 20;
ecs::ComponentDescriptorRegistrar<
    mono_meta_extension::MetaComponent,
    mono_meta_extension::ComponentAllocatorFactory,
    mono_meta_extension::MetaComponentHandle> g_meta_component_descriptor_registrar(
        g_component_descriptor_registry, MAX_META_COMPONENT_COUNT);

constexpr size_t MAX_ASSET_REQUEST_COMPONENT_COUNT = 10;
ecs::ComponentDescriptorRegistrar<
    mono_asset_extension::AssetRequestComponent,
    mono_asset_extension::ComponentAllocatorFactory,
    mono_asset_extension::AssetRequestComponentHandle> g_asset_request_component_descriptor_registrar(
        g_component_descriptor_registry, MAX_ASSET_REQUEST_COMPONENT_COUNT);

constexpr size_t MAX_TRANSFORM_COMPONENT_COUNT = 10;
ecs::ComponentDescriptorRegistrar<
    mono_transform_extension::TransformComponent,
    mono_transform_extension::ComponentAllocatorFactory,
    mono_transform_extension::TransformComponentHandle> g_transform_component_descriptor_registrar(
        g_component_descriptor_registry, MAX_TRANSFORM_COMPONENT_COUNT);

constexpr size_t MAX_SCENE_COMPONENT_COUNT = 5;
ecs::ComponentDescriptorRegistrar<
    mono_scene_extension::SceneComponent,
    mono_scene_extension::ComponentAllocatorFactory,
    mono_scene_extension::SceneComponentHandle> g_scene_component_descriptor_registrar(
        g_component_descriptor_registry, MAX_SCENE_COMPONENT_COUNT);

constexpr size_t MAX_SCENE_TAG_COMPONENT_COUNT = 20;
ecs::ComponentDescriptorRegistrar<
    mono_scene_extension::SceneTagComponent,
    mono_scene_extension::ComponentAllocatorFactory,
    mono_scene_extension::SceneTagComponentHandle> g_scene_tag_component_descriptor_registrar(
        g_component_descriptor_registry, MAX_SCENE_TAG_COMPONENT_COUNT);

constexpr size_t MAX_WINDOW_COMPONENT_COUNT = 5;
ecs::ComponentDescriptorRegistrar<
    mono_window_extension::WindowComponent,
    mono_window_extension::ComponentAllocatorFactory,
    mono_window_extension::WindowComponentHandle> g_window_component_descriptor_registrar(
        g_component_descriptor_registry, MAX_WINDOW_COMPONENT_COUNT);

constexpr size_t MAX_WINDOW_BIND_COMPONENT_COUNT = 5;
ecs::ComponentDescriptorRegistrar<
    mono_graphics_extension::WindowRenderBindComponent,
    mono_graphics_extension::ComponentAllocatorFactory,
    mono_graphics_extension::WindowRenderBindComponentHandle> g_window_render_bind_component_descriptor_registrar(
        g_component_descriptor_registry, MAX_WINDOW_BIND_COMPONENT_COUNT);

constexpr size_t MAX_CAMERA_COMPONENT_COUNT = 5;
ecs::ComponentDescriptorRegistrar<
    mono_graphics_extension::CameraComponent,
    mono_graphics_extension::ComponentAllocatorFactory,
    mono_graphics_extension::CameraComponentHandle> g_camera_component_descriptor_registrar(
        g_component_descriptor_registry, MAX_CAMERA_COMPONENT_COUNT);

constexpr size_t MAX_RENDERABLE_COMPONENT_COUNT = 10;
ecs::ComponentDescriptorRegistrar<
    mono_graphics_extension::RenderableComponent,
    mono_graphics_extension::ComponentAllocatorFactory,
    mono_graphics_extension::RenderableComponentHandle> g_renderable_component_descriptor_registrar(
        g_component_descriptor_registry, MAX_RENDERABLE_COMPONENT_COUNT);

constexpr size_t MAX_UI_COMPONENT_COUNT = 10;
ecs::ComponentDescriptorRegistrar<
    mono_graphics_extension::UIComponent,
    mono_graphics_extension::ComponentAllocatorFactory,
    mono_graphics_extension::UIComponentHandle> g_ui_component_descriptor_registrar(
        g_component_descriptor_registry, MAX_UI_COMPONENT_COUNT);

constexpr size_t MAX_EDITOR_ENABLED_COMPONENT_COUNT = 10;
ecs::ComponentDescriptorRegistrar<
    mono_entity_archive_extension::EditorEnabledComponent,
    mono_entity_archive_extension::ComponentAllocatorFactory,
    mono_entity_archive_extension::EditorEnabledComponentHandle> g_editor_enabled_component_descriptor_registrar(
        g_component_descriptor_registry, MAX_EDITOR_ENABLED_COMPONENT_COUNT);

constexpr size_t MAX_HIERARCHY_UI_COMPONENT_COUNT = 5;
ecs::ComponentDescriptorRegistrar<
    mono_entity_archive_extension::HierarchyUIComponent,
    mono_entity_archive_extension::ComponentAllocatorFactory,
    mono_entity_archive_extension::HierarchyUIComponentHandle> g_hierarchy_ui_component_descriptor_registrar(
        g_component_descriptor_registry, MAX_HIERARCHY_UI_COMPONENT_COUNT);

constexpr size_t MAX_INSPECTOR_UI_COMPONENT_COUNT = 5;
ecs::ComponentDescriptorRegistrar<
    mono_entity_archive_extension::InspectorUIComponent,
    mono_entity_archive_extension::ComponentAllocatorFactory,
    mono_entity_archive_extension::InspectorUIComponentHandle> g_inspector_ui_component_descriptor_registrar(
        g_component_descriptor_registry, MAX_INSPECTOR_UI_COMPONENT_COUNT);

constexpr size_t MAX_ASSET_BROWSER_UI_COMPONENT_COUNT = 5;
ecs::ComponentDescriptorRegistrar<
    mono_entity_archive_extension::AssetBrowserUIComponent,
    mono_entity_archive_extension::ComponentAllocatorFactory,
    mono_entity_archive_extension::AssetBrowserUIComponentHandle> g_asset_browser_ui_component_descriptor_registrar(
        g_component_descriptor_registry, MAX_ASSET_BROWSER_UI_COMPONENT_COUNT);

constexpr size_t MAX_TRANSFORM_MANIPULATOR_UI_COMPONENT_COUNT = 5;
ecs::ComponentDescriptorRegistrar<
    mono_entity_archive_extension::TransformManipulatorUIComponent,
    mono_entity_archive_extension::ComponentAllocatorFactory,
    mono_entity_archive_extension::TransformManipulatorUIComponentHandle> g_transform_manipulator_ui_component_descriptor_registrar(
        g_component_descriptor_registry, MAX_TRANSFORM_MANIPULATOR_UI_COMPONENT_COUNT);

constexpr size_t MAX_MENU_BAR_UI_COMPONENT_COUNT = 2;
ecs::ComponentDescriptorRegistrar<
    mono_entity_archive_extension::MenuBarUIComponent,
    mono_entity_archive_extension::ComponentAllocatorFactory,
    mono_entity_archive_extension::MenuBarUIComponentHandle> g_menu_bar_ui_component_descriptor_registrar(
        g_component_descriptor_registry, MAX_MENU_BAR_UI_COMPONENT_COUNT);

constexpr size_t MAX_MATERIAL_EDITOR_UI_COMPONENT_COUNT = 2;
ecs::ComponentDescriptorRegistrar<
    mono_entity_archive_extension::MaterialEditorUIComponent,
    mono_entity_archive_extension::ComponentAllocatorFactory,
    mono_entity_archive_extension::MaterialEditorUIComponentHandle> g_material_editor_ui_component_descriptor_registrar(
        g_component_descriptor_registry, MAX_MATERIAL_EDITOR_UI_COMPONENT_COUNT);

constexpr size_t MAX_DIRECTIONAL_LIGHT_COMPONENT_COUNT = 5;
ecs::ComponentDescriptorRegistrar<
    mono_graphics_extension::DirectionalLightComponent,
    mono_graphics_extension::ComponentAllocatorFactory,
    mono_graphics_extension::DirectionalLightComponentHandle> g_directional_light_component_descriptor_registrar(
        g_component_descriptor_registry, MAX_DIRECTIONAL_LIGHT_COMPONENT_COUNT);

constexpr size_t MAX_AMBIENT_LIGHT_COMPONENT_COUNT = 5;
ecs::ComponentDescriptorRegistrar<
    mono_graphics_extension::AmbientLightComponent,
    mono_graphics_extension::ComponentAllocatorFactory,
    mono_graphics_extension::AmbientLightComponentHandle> g_ambient_light_component_descriptor_registrar(
        g_component_descriptor_registry, MAX_AMBIENT_LIGHT_COMPONENT_COUNT);

constexpr size_t MAX_POINT_LIGHT_COMPONENT_COUNT = 10;
ecs::ComponentDescriptorRegistrar<
    mono_graphics_extension::PointLightComponent,
    mono_graphics_extension::ComponentAllocatorFactory,
    mono_graphics_extension::PointLightComponentHandle> g_point_light_component_descriptor_registrar(
        g_component_descriptor_registry, MAX_POINT_LIGHT_COMPONENT_COUNT);

//COMPONENT_DESCRIPTOR_REGISTRAR_DEFINE_END//

} // namespace mono_forge