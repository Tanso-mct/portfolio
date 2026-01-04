#include "mono_forge/src/pch.h"
#include "mono_forge/include/materials.h"

#include "imgui/include/imgui.h"
#include "imgui/include/imgui_internal.h"

#include "mono_forge/include/assets.h"

#include "render_graph/include/lambert_material.h"
#include "render_graph/include/phong_material.h"
#include "mono_graphics_service/include/graphics_command_list.h"

#include "mono_entity_archive_service/include/entity_archive_service_command_list.h"
#include "mono_entity_archive_extension/include/material_additional.h"

#include "mono_asset_service/include/asset_service_view.h"
#include "mono_asset_extension/include/texture_asset.h"

namespace mono_forge
{

render_graph::MaterialTypeHandleIDGenerator material_type_handle_id_generator;
mono_entity_archive_service::MaterialSetupParamEditorRegistry g_material_setup_param_editor_registry;

// Helper function to create a combo box with label and update source value
void ComboWithLabel(
    bool& edited, const char* label, uint32_t* source, const std::vector<const char*>& items)
{
    int current_item = static_cast<int>(*source);
    if (ImGui::Combo(label, &current_item, items.data(), items.size()))
    {
        *source = static_cast<uint32_t>(current_item);
        edited = true;
    }
}

// Helper function to create a combo box for texture asset selection
asset_loader::AssetHandleID ComboWithLabel(
    bool& edited, const char* label, const asset_loader::AssetHandleID& current_asset_id,
    const std::vector<asset_loader::AssetHandleID>& texture_asset_ids, 
    std::vector<const char*> texture_asset_name_items)
{
    // Find current index
    size_t current_index = 0;
    for (size_t i = 0; i < texture_asset_ids.size(); ++i)
    {
        // Get texture asset id
        const asset_loader::AssetHandleID& asset_id = texture_asset_ids[i];

        if (asset_id == current_asset_id) // Found
        {
            current_index = i;
            break;
        }
    }

    if (ImGui::Combo(
        label, reinterpret_cast<int*>(&current_index), 
        texture_asset_name_items.data(), static_cast<int>(texture_asset_name_items.size())))
    {
        // Update albedo texture asset ID
        edited = true;
        return texture_asset_ids[current_index];
    }

    return current_asset_id; // No change
}

// Helper function to update texture handle from asset
void UpdateTextureHandle(const asset_loader::Asset& asset, const render_graph::ResourceHandle** dist_texture_handle)
{
    // Cast to texture asset
    const mono_asset_extension::TextureAsset* texture_asset_ptr
        = dynamic_cast<const mono_asset_extension::TextureAsset*>(&asset);
    assert(texture_asset_ptr != nullptr && "Albedo texture asset is not a texture asset");

    // Set the texture handle
    *dist_texture_handle = texture_asset_ptr->GetTextureHandle();
}

//MATERIAL_SETUP_PARAM_EDITOR_REGISTRAR_BEGIN//

mono_entity_archive_service::MaterialSetupParamEditorRegistrar g_lambert_material_editor_registrar(
    g_material_setup_param_editor_registry,
    render_graph::LambertMaterialTypeHandle::ID(), "Lambert Material",
    [](
        const material_editor::SetupParamWrapper* setup_param,
        mono_service::ServiceProxyManager& service_proxy_manager) -> std::unique_ptr<material_editor::SetupParamWrapper>
    {
        // Clone the setup param
        std::unique_ptr<render_graph::Material::SetupParam> edited_param = setup_param->GetSetupParam()->Clone();
        std::unique_ptr<material_editor::AdditionalSetupParam> edited_additional_param = setup_param->GetAdditionalParam()->Clone();

        // Cast to LambertMaterial::SetupParam
        render_graph::LambertMaterial::SetupParam* edited_param_ptr
            = dynamic_cast<render_graph::LambertMaterial::SetupParam*>(edited_param.get());
        assert(edited_param_ptr != nullptr && "Invalid setup param type for Lambert material");

        // Cast to LambertMaterialAdditionalSetupParam
        mono_entity_archive_extension::LambertMaterialAdditionalSetupParam* edited_additional_param_ptr
            = dynamic_cast<mono_entity_archive_extension::LambertMaterialAdditionalSetupParam*>(edited_additional_param.get());
        assert(edited_additional_param_ptr != nullptr && "Invalid additional setup param type for Lambert material");

        // Indicate whether the param was edited
        bool edited = false;

        // Base color
        edited |= ImGui::ColorEdit4(
            "Base Color", reinterpret_cast<float*>(&edited_param_ptr->base_color), ImGuiColorEditFlags_None);

        // Albedo source
        ComboWithLabel(
            edited, "Albedo Source", &edited_param_ptr->albedo_source,
            { "Base Color", "Texture" });
        
        // Normal source
        ComboWithLabel(
            edited, "Normal Source", &edited_param_ptr->normal_source,
            { "Vertex", "Texture" });

        // AO source
        ComboWithLabel(
            edited, "AO Source", &edited_param_ptr->ao_source,
            { "None", "Texture" });

        // Emission source
        ComboWithLabel(
            edited, "Emission Source", &edited_param_ptr->emission_source,
            { "None", "Texture" });

        // Emission color
        edited |= ImGui::ColorEdit4(
            "Emission Color", reinterpret_cast<float*>(&edited_param_ptr->emission_color), ImGuiColorEditFlags_None);

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

        // Cast to asset service view
        mono_asset_service::AssetServiceView* asset_service_view_ptr
            = dynamic_cast<mono_asset_service::AssetServiceView*>(asset_service_view.get());
        assert(asset_service_view_ptr != nullptr && "Asset service view is null!");

        // Get loaded asset IDs
        std::vector<asset_loader::AssetHandleID> loaded_asset_ids = asset_service_view_ptr->GetLoadedAssetIDs();

        // Collect texture asset IDs
        std::vector<asset_loader::AssetHandleID> texture_asset_ids;
        std::vector<const char*> texture_asset_name_items;
        for (const asset_loader::AssetHandleID& asset_id : loaded_asset_ids)
        {
            // Get asset
            const asset_loader::Asset& asset = asset_service_view_ptr->GetAsset(asset_id);

            // Try cast to texture asset
            const mono_asset_extension::TextureAsset* texture_asset_ptr
                = dynamic_cast<const mono_asset_extension::TextureAsset*>(&asset);

            if (texture_asset_ptr != nullptr)
            {
                // Add asset ID to texture asset IDs
                texture_asset_ids.push_back(asset_id);

                // Add name to items
                texture_asset_name_items.push_back(asset.GetName().data());
            }
        }

        // Albedo texture
        asset_loader::AssetHandleID new_albedo_texture_asset_id = ComboWithLabel(
            edited, "Albedo Texture", edited_additional_param_ptr->GetAlbedoTextureAssetID(),
            texture_asset_ids, texture_asset_name_items);
        edited_additional_param_ptr->SetAlbedoTextureAssetID(new_albedo_texture_asset_id);
        UpdateTextureHandle(
            asset_service_view_ptr->GetAsset(new_albedo_texture_asset_id),
            &edited_param_ptr->albedo_texture_handle);

        // Normal texture
        asset_loader::AssetHandleID new_normal_texture_asset_id = ComboWithLabel(
            edited, "Normal Texture", edited_additional_param_ptr->GetNormalTextureAssetID(),
            texture_asset_ids, texture_asset_name_items);
        edited_additional_param_ptr->SetNormalTextureAssetID(new_normal_texture_asset_id);
        UpdateTextureHandle(
            asset_service_view_ptr->GetAsset(new_normal_texture_asset_id),
            &edited_param_ptr->normal_texture_handle);

        // AO texture
        asset_loader::AssetHandleID new_ao_texture_asset_id = ComboWithLabel(
            edited, "AO Texture", edited_additional_param_ptr->GetAOTextureAssetID(),
            texture_asset_ids, texture_asset_name_items);
        edited_additional_param_ptr->SetAOTextureAssetID(new_ao_texture_asset_id);
        UpdateTextureHandle(
            asset_service_view_ptr->GetAsset(new_ao_texture_asset_id),
            &edited_param_ptr->ao_texture_handle);

        // Emission texture
        asset_loader::AssetHandleID new_emission_texture_asset_id = ComboWithLabel(
            edited, "Emission Texture", edited_additional_param_ptr->GetEmissionTextureAssetID(),
            texture_asset_ids, texture_asset_name_items);
        edited_additional_param_ptr->SetEmissionTextureAssetID(new_emission_texture_asset_id);
        UpdateTextureHandle(
            asset_service_view_ptr->GetAsset(new_emission_texture_asset_id),
            &edited_param_ptr->emission_texture_handle);

        if (edited)
        {
            return std::make_unique<material_editor::SetupParamWrapper>(
                std::move(edited_param), std::move(edited_additional_param)); // Return edited param
        }

        return nullptr; // Indicate not edited
    },
    [](mono_service::ServiceProxyManager& service_proxy_manager) -> bool
    {
        // Get asset service proxy, graphics service proxy, entity archive service proxy
        std::unique_ptr<mono_service::ServiceProxy> asset_service_proxy = nullptr;
        std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy = nullptr;
        std::unique_ptr<mono_service::ServiceProxy> entity_archive_service_proxy = nullptr;
        service_proxy_manager.WithLock([&](mono_service::ServiceProxyManager& service_proxy_manager_inner)
        {
            asset_service_proxy 
                = service_proxy_manager_inner.GetServiceProxy(mono_asset_service::AssetServiceHandle::ID()).Clone();

            graphics_service_proxy 
                = service_proxy_manager_inner.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID()).Clone();

            entity_archive_service_proxy 
                = service_proxy_manager_inner.GetServiceProxy(mono_entity_archive_service::EntityArchiveServiceHandle::ID()).Clone();
        });

        // Create asset service view
        std::unique_ptr<mono_service::ServiceView> asset_service_view = asset_service_proxy->CreateView();
        mono_asset_service::AssetServiceView* asset_service_view_ptr
            = dynamic_cast<mono_asset_service::AssetServiceView*>(asset_service_view.get());
        assert(asset_service_view_ptr != nullptr && "Asset service view is null!");

        // Create graphics service command list
        std::unique_ptr<mono_service::ServiceCommandList> graphics_command_list = graphics_service_proxy->CreateCommandList();
        mono_graphics_service::GraphicsCommandList* graphics_command_list_ptr
            = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(graphics_command_list.get());
        assert(graphics_command_list_ptr != nullptr && "Graphics command list is null!");

        // Create entity archive service command list
        std::unique_ptr<mono_service::ServiceCommandList> entity_archive_command_list = entity_archive_service_proxy->CreateCommandList();
        mono_entity_archive_service::EntityArchiveServiceCommandList* entity_archive_command_list_ptr
            = dynamic_cast<mono_entity_archive_service::EntityArchiveServiceCommandList*>(entity_archive_command_list.get());
        assert(entity_archive_command_list_ptr != nullptr && "Entity archive service command list is null!");

        // Get material handle manager
        render_graph::MaterialHandleManager& material_handle_manager
            = render_graph::MaterialHandleManager::GetInstance();

        // Get empty texture asset
        const asset_loader::Asset& empty_texture_asset
            = asset_service_view_ptr->GetAsset(EmptyTextureAssetHandle::ID());
        const mono_asset_extension::TextureAsset* empty_texture_asset_ptr
            = dynamic_cast<const mono_asset_extension::TextureAsset*>(&empty_texture_asset);

        // Create box lambert material setup parameter
        std::unique_ptr<render_graph::LambertMaterial::SetupParam> lambert_material_param
            = std::make_unique<render_graph::LambertMaterial::SetupParam>();
        lambert_material_param->base_color = BOX_DEFAULT_BASE_COLOR;
        lambert_material_param->albedo_source = render_graph::LambertMaterial::ALBEDO_SOURCE_BASE_COLOR;
        lambert_material_param->normal_source = render_graph::LambertMaterial::NORMAL_SOURCE_VERTEX;
        lambert_material_param->ao_source = render_graph::LambertMaterial::AO_SOURCE_NONE;
        lambert_material_param->emission_source = render_graph::LambertMaterial::EMISSION_SOURCE_NONE;
        lambert_material_param->emission_color = BOX_DEFAULT_EMISSION_COLOR;
        lambert_material_param->albedo_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        lambert_material_param->normal_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        lambert_material_param->ao_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        lambert_material_param->emission_texture_handle = empty_texture_asset_ptr->GetTextureHandle();

        // Generate new material handle key
        render_graph::MaterialHandleKey new_key = render_graph::MaterialHandleKeyGenerator::GetInstance().Generate();

        // Register material handle
        material_handle_manager.RegisterMaterialHandle(new_key);

        // Create lambert material
        graphics_command_list_ptr->CreateMaterial<render_graph::LambertMaterial>(
            material_handle_manager.GetMaterialHandle(new_key), 
            std::move(lambert_material_param));

        // Create setup param for entity archive service again because it was moved
        lambert_material_param
            = std::make_unique<render_graph::LambertMaterial::SetupParam>();
        lambert_material_param->base_color = BOX_DEFAULT_BASE_COLOR;
        lambert_material_param->albedo_source = render_graph::LambertMaterial::ALBEDO_SOURCE_BASE_COLOR;
        lambert_material_param->normal_source = render_graph::LambertMaterial::NORMAL_SOURCE_VERTEX;
        lambert_material_param->ao_source = render_graph::LambertMaterial::AO_SOURCE_NONE;
        lambert_material_param->emission_source = render_graph::LambertMaterial::EMISSION_SOURCE_NONE;
        lambert_material_param->emission_color = BOX_DEFAULT_EMISSION_COLOR;
        lambert_material_param->albedo_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        lambert_material_param->normal_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        lambert_material_param->ao_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        lambert_material_param->emission_texture_handle = empty_texture_asset_ptr->GetTextureHandle();

        // Create lambert material additional setup param for entity archive service
        std::unique_ptr<mono_entity_archive_extension::LambertMaterialAdditionalSetupParam> lambert_material_additional_param
            = std::make_unique<mono_entity_archive_extension::LambertMaterialAdditionalSetupParam>(
                EmptyTextureAssetHandle::ID(), // Albedo
                EmptyTextureAssetHandle::ID(), // Normal
                EmptyTextureAssetHandle::ID(), // AO
                EmptyTextureAssetHandle::ID()); // Emission

        // Add setup param to entity archive service
        entity_archive_command_list_ptr->AddSetupParam(
            material_handle_manager.GetMaterialHandle(new_key), 
            std::make_unique<material_editor::SetupParamWrapper>(
                std::move(lambert_material_param),
                std::move(lambert_material_additional_param)));

        // Submit command lists
        graphics_service_proxy->SubmitCommandList(std::move(graphics_command_list));
        entity_archive_service_proxy->SubmitCommandList(std::move(entity_archive_command_list));

        return true; // Indicate created
    });

mono_entity_archive_service::MaterialSetupParamEditorRegistrar g_phong_material_editor_registrar(
    g_material_setup_param_editor_registry,
    render_graph::PhongMaterialTypeHandle::ID(), "Phong Material",
    [](
        const material_editor::SetupParamWrapper* setup_param,
        mono_service::ServiceProxyManager& service_proxy_manager) -> std::unique_ptr<material_editor::SetupParamWrapper>
    {
        // Clone the setup param
        std::unique_ptr<render_graph::Material::SetupParam> edited_param = setup_param->GetSetupParam()->Clone();
        std::unique_ptr<material_editor::AdditionalSetupParam> edited_additional_param = setup_param->GetAdditionalParam()->Clone();

        // Cast to PhongMaterial::SetupParam
        render_graph::PhongMaterial::SetupParam* edited_param_ptr
            = dynamic_cast<render_graph::PhongMaterial::SetupParam*>(edited_param.get());
        assert(edited_param_ptr != nullptr && "Invalid setup param type for Phong material");

        // Cast to PhongMaterialAdditionalSetupParam
        mono_entity_archive_extension::PhongMaterialAdditionalSetupParam* edited_additional_param_ptr   
            = dynamic_cast<mono_entity_archive_extension::PhongMaterialAdditionalSetupParam*>(edited_additional_param.get());
        assert(edited_additional_param_ptr != nullptr && "Invalid additional setup param type for Phong material");

        // Indicate whether the param was edited
        bool edited = false;

        // Base color
        edited |= ImGui::ColorEdit4(
            "Base Color", reinterpret_cast<float*>(&edited_param_ptr->base_color), ImGuiColorEditFlags_None);

        // Albedo source
        ComboWithLabel(
            edited, "Albedo Source", &edited_param_ptr->albedo_source,
            { "Base Color", "Texture" });

        // Normal source
        ComboWithLabel(
            edited, "Normal Source", &edited_param_ptr->normal_source,
            { "Vertex", "Texture" });

        // AO source
        ComboWithLabel(
            edited, "AO Source", &edited_param_ptr->ao_source,
            { "None", "Texture" });

        // Roughness source
        ComboWithLabel(
            edited, "Roughness Source", &edited_param_ptr->roughness_source,
            { "Value", "Texture" });

        // Roughness value
        edited |= ImGui::DragFloat(
            "Roughness Value", &edited_param_ptr->roughness_value, 0.01f, 0.0f, 1.0f);
        
        // Metalness source
        ComboWithLabel(
            edited, "Metalness Source", &edited_param_ptr->metalness_source,
            { "Value", "Texture" });

        // Metalness value
        edited |= ImGui::DragFloat(
            "Metalness Value", &edited_param_ptr->metalness_value, 0.01f, 0.0f, 1.0f);

        // Emission source
        ComboWithLabel(
            edited, "Emission Source", &edited_param_ptr->emission_source,
            { "Color", "Texture" });

        // Emission color
        edited |= ImGui::ColorEdit4(
            "Emission Color", reinterpret_cast<float*>(&edited_param_ptr->emission_color), ImGuiColorEditFlags_None);

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

        // Cast to asset service view
        mono_asset_service::AssetServiceView* asset_service_view_ptr
            = dynamic_cast<mono_asset_service::AssetServiceView*>(asset_service_view.get());
        assert(asset_service_view_ptr != nullptr && "Asset service view is null!");

        // Get loaded asset IDs
        std::vector<asset_loader::AssetHandleID> loaded_asset_ids = asset_service_view_ptr->GetLoadedAssetIDs();

        // Collect texture asset IDs
        std::vector<asset_loader::AssetHandleID> texture_asset_ids;
        std::vector<const char*> texture_asset_name_items;
        for (const asset_loader::AssetHandleID& asset_id : loaded_asset_ids)
        {
            // Get asset
            const asset_loader::Asset& asset = asset_service_view_ptr->GetAsset(asset_id);

            // Try cast to texture asset
            const mono_asset_extension::TextureAsset* texture_asset_ptr
                = dynamic_cast<const mono_asset_extension::TextureAsset*>(&asset);

            if (texture_asset_ptr != nullptr)
            {
                // Add asset ID to texture asset IDs
                texture_asset_ids.push_back(asset_id);

                // Add name to items
                texture_asset_name_items.push_back(asset.GetName().data());
            }
        }

        // Albedo texture
        asset_loader::AssetHandleID new_albedo_texture_asset_id = ComboWithLabel(
            edited, "Albedo Texture", edited_additional_param_ptr->GetAlbedoTextureAssetID(),
            texture_asset_ids, texture_asset_name_items);
        edited_additional_param_ptr->SetAlbedoTextureAssetID(new_albedo_texture_asset_id);
        UpdateTextureHandle(
            asset_service_view_ptr->GetAsset(new_albedo_texture_asset_id),
            &edited_param_ptr->albedo_texture_handle);

        // Normal texture
        asset_loader::AssetHandleID new_normal_texture_asset_id = ComboWithLabel(
            edited, "Normal Texture", edited_additional_param_ptr->GetNormalTextureAssetID(),
            texture_asset_ids, texture_asset_name_items);
        edited_additional_param_ptr->SetNormalTextureAssetID(new_normal_texture_asset_id);
        UpdateTextureHandle(
            asset_service_view_ptr->GetAsset(new_normal_texture_asset_id),
            &edited_param_ptr->normal_texture_handle);

        // AO texture
        asset_loader::AssetHandleID new_ao_texture_asset_id = ComboWithLabel(
            edited, "AO Texture", edited_additional_param_ptr->GetAOTextureAssetID(),
            texture_asset_ids, texture_asset_name_items);
        edited_additional_param_ptr->SetAOTextureAssetID(new_ao_texture_asset_id);
        UpdateTextureHandle(
            asset_service_view_ptr->GetAsset(new_ao_texture_asset_id),
            &edited_param_ptr->ao_texture_handle);

        // Roughness texture
        asset_loader::AssetHandleID new_roughness_texture_asset_id = ComboWithLabel(
            edited, "Roughness Texture", edited_additional_param_ptr->GetRoughnessTextureAssetID(),
            texture_asset_ids, texture_asset_name_items);
        edited_additional_param_ptr->SetRoughnessTextureAssetID(new_roughness_texture_asset_id);
        UpdateTextureHandle(
            asset_service_view_ptr->GetAsset(new_roughness_texture_asset_id),
            &edited_param_ptr->roughness_texture_handle);

        // Metalness texture
        asset_loader::AssetHandleID new_metalness_texture_asset_id = ComboWithLabel(
            edited, "Metalness Texture", edited_additional_param_ptr->GetMetalnessTextureAssetID(),
            texture_asset_ids, texture_asset_name_items);
        edited_additional_param_ptr->SetMetalnessTextureAssetID(new_metalness_texture_asset_id);
        UpdateTextureHandle(
            asset_service_view_ptr->GetAsset(new_metalness_texture_asset_id),
            &edited_param_ptr->metalness_texture_handle);

        // Emission texture
        asset_loader::AssetHandleID new_emission_texture_asset_id = ComboWithLabel(
            edited, "Emission Texture", edited_additional_param_ptr->GetEmissionTextureAssetID(),
            texture_asset_ids, texture_asset_name_items);
        edited_additional_param_ptr->SetEmissionTextureAssetID(new_emission_texture_asset_id);
        UpdateTextureHandle(
            asset_service_view_ptr->GetAsset(new_emission_texture_asset_id),
            &edited_param_ptr->emission_texture_handle);

        if (edited)
        {
            return std::make_unique<material_editor::SetupParamWrapper>(
                std::move(edited_param), std::move(edited_additional_param)); // Return edited param
        }
        
        return nullptr; // Indicate not edited
    },
    [](mono_service::ServiceProxyManager& service_proxy_manager) -> bool
    {
        // Get asset service proxy, graphics service proxy, entity archive service proxy
        std::unique_ptr<mono_service::ServiceProxy> asset_service_proxy = nullptr;
        std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy = nullptr;
        std::unique_ptr<mono_service::ServiceProxy> entity_archive_service_proxy = nullptr;
        service_proxy_manager.WithLock([&](mono_service::ServiceProxyManager& service_proxy_manager_inner)
        {
            asset_service_proxy 
                = service_proxy_manager_inner.GetServiceProxy(mono_asset_service::AssetServiceHandle::ID()).Clone();

            graphics_service_proxy 
                = service_proxy_manager_inner.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID()).Clone();

            entity_archive_service_proxy 
                = service_proxy_manager_inner.GetServiceProxy(mono_entity_archive_service::EntityArchiveServiceHandle::ID()).Clone();
        });

        // Create asset service view
        std::unique_ptr<mono_service::ServiceView> asset_service_view = asset_service_proxy->CreateView();
        mono_asset_service::AssetServiceView* asset_service_view_ptr
            = dynamic_cast<mono_asset_service::AssetServiceView*>(asset_service_view.get());
        assert(asset_service_view_ptr != nullptr && "Asset service view is null!");

        // Create graphics service command list
        std::unique_ptr<mono_service::ServiceCommandList> graphics_command_list = graphics_service_proxy->CreateCommandList();
        mono_graphics_service::GraphicsCommandList* graphics_command_list_ptr
            = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(graphics_command_list.get());
        assert(graphics_command_list_ptr != nullptr && "Graphics command list is null!");

        // Create entity archive service command list
        std::unique_ptr<mono_service::ServiceCommandList> entity_archive_command_list = entity_archive_service_proxy->CreateCommandList();
        mono_entity_archive_service::EntityArchiveServiceCommandList* entity_archive_command_list_ptr
            = dynamic_cast<mono_entity_archive_service::EntityArchiveServiceCommandList*>(entity_archive_command_list.get());
        assert(entity_archive_command_list_ptr != nullptr && "Entity archive service command list is null!");

        // Get material handle manager
        render_graph::MaterialHandleManager& material_handle_manager
            = render_graph::MaterialHandleManager::GetInstance();

        // Get empty texture asset
        const asset_loader::Asset& empty_texture_asset
            = asset_service_view_ptr->GetAsset(EmptyTextureAssetHandle::ID());
        const mono_asset_extension::TextureAsset* empty_texture_asset_ptr
            = dynamic_cast<const mono_asset_extension::TextureAsset*>(&empty_texture_asset);
        assert(empty_texture_asset_ptr != nullptr && "Failed to get empty texture asset!");

        // Create phong material setup parameter
        std::unique_ptr<render_graph::PhongMaterial::SetupParam> phong_material_param
            = std::make_unique<render_graph::PhongMaterial::SetupParam>();
        phong_material_param->base_color = BOX_DEFAULT_BASE_COLOR;
        phong_material_param->albedo_source = render_graph::PhongMaterial::ALBEDO_SOURCE_BASE_COLOR;
        phong_material_param->normal_source = render_graph::PhongMaterial::NORMAL_SOURCE_TEXTURE;
        phong_material_param->ao_source = render_graph::PhongMaterial::AO_SOURCE_NONE;
        phong_material_param->specular_source = render_graph::PhongMaterial::SPECULAR_SOURCE_NONE;
        phong_material_param->roughness_source = render_graph::PhongMaterial::ROUGHNESS_SOURCE_VALUE;
        phong_material_param->roughness_value = BOX_DEFAULT_ROUGHNESS_VALUE;
        phong_material_param->metalness_source = render_graph::PhongMaterial::METALNESS_SOURCE_VALUE;
        phong_material_param->metalness_value = BOX_DEFAULT_METALNESS_VALUE;
        phong_material_param->emission_source = render_graph::PhongMaterial::EMISSION_SOURCE_COLOR;
        phong_material_param->emission_color = BOX_DEFAULT_EMISSION_COLOR;
        phong_material_param->albedo_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        phong_material_param->normal_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        phong_material_param->ao_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        phong_material_param->specular_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        phong_material_param->roughness_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        phong_material_param->metalness_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        phong_material_param->emission_texture_handle = empty_texture_asset_ptr->GetTextureHandle();

        // Generate new material handle key
        render_graph::MaterialHandleKey new_key = render_graph::MaterialHandleKeyGenerator::GetInstance().Generate();

        // Register material handle
        material_handle_manager.RegisterMaterialHandle(new_key);

        // Create marble bust phong material
        graphics_command_list_ptr->CreateMaterial<render_graph::PhongMaterial>(
            material_handle_manager.GetMaterialHandle(new_key), 
            std::move(phong_material_param));

        // Create marble bust lambert material setup parameter again because it was moved
        phong_material_param
            = std::make_unique<render_graph::PhongMaterial::SetupParam>();
        phong_material_param->base_color = BOX_DEFAULT_BASE_COLOR;
        phong_material_param->albedo_source = render_graph::PhongMaterial::ALBEDO_SOURCE_BASE_COLOR;
        phong_material_param->normal_source = render_graph::PhongMaterial::NORMAL_SOURCE_TEXTURE;
        phong_material_param->ao_source = render_graph::PhongMaterial::AO_SOURCE_NONE;
        phong_material_param->specular_source = render_graph::PhongMaterial::SPECULAR_SOURCE_NONE;
        phong_material_param->roughness_source = render_graph::PhongMaterial::ROUGHNESS_SOURCE_VALUE;
        phong_material_param->roughness_value = BOX_DEFAULT_ROUGHNESS_VALUE;
        phong_material_param->metalness_source = render_graph::PhongMaterial::METALNESS_SOURCE_VALUE;
        phong_material_param->metalness_value = BOX_DEFAULT_METALNESS_VALUE;
        phong_material_param->emission_source = render_graph::PhongMaterial::EMISSION_SOURCE_COLOR;
        phong_material_param->emission_color = BOX_DEFAULT_EMISSION_COLOR;
        phong_material_param->albedo_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        phong_material_param->normal_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        phong_material_param->ao_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        phong_material_param->specular_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        phong_material_param->roughness_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        phong_material_param->metalness_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        phong_material_param->emission_texture_handle = empty_texture_asset_ptr->GetTextureHandle();

        // Create phong material additional setup param for entity archive service
        std::unique_ptr<mono_entity_archive_extension::PhongMaterialAdditionalSetupParam> phong_material
            = std::make_unique<mono_entity_archive_extension::PhongMaterialAdditionalSetupParam>(
                EmptyTextureAssetHandle::ID(), // Albedo
                EmptyTextureAssetHandle::ID(), // Normal
                EmptyTextureAssetHandle::ID(), // AO
                EmptyTextureAssetHandle::ID(), // Specular
                EmptyTextureAssetHandle::ID(), // Roughness
                EmptyTextureAssetHandle::ID(), // Metalness
                EmptyTextureAssetHandle::ID()); // Emission

        // Add setup param to entity archive service
        entity_archive_command_list_ptr->AddSetupParam(
            material_handle_manager.GetMaterialHandle(new_key), 
            std::make_unique<material_editor::SetupParamWrapper>(
                std::move(phong_material_param),
                std::move(phong_material)));

        // Submit command lists
        graphics_service_proxy->SubmitCommandList(std::move(graphics_command_list));
        entity_archive_service_proxy->SubmitCommandList(std::move(entity_archive_command_list));

        return true; // Indicate created
    });

//MATERIAL_SETUP_PARAM_EDITOR_REGISTRAR_END//

} // namespace mono_forge