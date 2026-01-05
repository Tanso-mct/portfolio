#include "mono_forge_app_template/src/pch.h"
#include "mono_forge_app_template/include/materials.h"

#include "utility_header/win32.h"

#include "imgui/include/imgui.h"
#include "imgui/include/imgui_internal.h"

#include "mono_forge_app_template/include/assets.h"

#include "render_graph/include/lambert_material.h"
#include "render_graph/include/phong_material.h"
#include "mono_graphics_service/include/graphics_command_list.h"

#include "mono_entity_archive_service/include/entity_archive_service_command_list.h"
#include "mono_entity_archive_extension/include/material_additional.h"

#include "mono_asset_service/include/asset_service_view.h"
#include "mono_asset_extension/include/texture_asset.h"

namespace mono_forge_app_template
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
    },
    [](
        const material_editor::SetupParamWrapper* setup_param,
        mono_service::ServiceProxyManager& service_proxy_manager) -> nlohmann::json
    {
        // Cast to LambertMaterial::SetupParam
        const render_graph::LambertMaterial::SetupParam* lambert_param_ptr
            = dynamic_cast<const render_graph::LambertMaterial::SetupParam*>(setup_param->GetSetupParam());
        assert(lambert_param_ptr != nullptr && "Invalid setup param type for Lambert material");

        // Cast to LambertMaterialAdditionalSetupParam
        const mono_entity_archive_extension::LambertMaterialAdditionalSetupParam* lambert_additional_param_ptr
            = dynamic_cast<const mono_entity_archive_extension::LambertMaterialAdditionalSetupParam*>(
                setup_param->GetAdditionalParam());
        assert(lambert_additional_param_ptr != nullptr && "Invalid additional setup param type for Lambert material");

        // Get asset service proxy, graphics service proxy, entity archive service proxy
        std::unique_ptr<mono_service::ServiceProxy> asset_service_proxy = nullptr;
        service_proxy_manager.WithLock([&](mono_service::ServiceProxyManager& service_proxy_manager_inner)
        {
            asset_service_proxy 
                = service_proxy_manager_inner.GetServiceProxy(mono_asset_service::AssetServiceHandle::ID()).Clone();
        });

        // Create asset service view
        std::unique_ptr<mono_service::ServiceView> asset_service_view = asset_service_proxy->CreateView();
        mono_asset_service::AssetServiceView* asset_service_view_ptr
            = dynamic_cast<mono_asset_service::AssetServiceView*>(asset_service_view.get());
        assert(asset_service_view_ptr != nullptr && "Asset service view is null!");

        // Get material handle manager
        render_graph::MaterialHandleManager& material_handle_manager
            = render_graph::MaterialHandleManager::GetInstance();

        // Create JSON object
        nlohmann::json json_obj;

        // Debug name
        json_obj[render_graph::LAMBERT_MATERIAL_TYPE_NAME]["debug_name_prefix"] = lambert_param_ptr->debug_name_prefix;

        // Base color
        json_obj[render_graph::LAMBERT_MATERIAL_TYPE_NAME]["base_color"] = {
            lambert_param_ptr->base_color.x,
            lambert_param_ptr->base_color.y,
            lambert_param_ptr->base_color.z,
            lambert_param_ptr->base_color.w};

        // Albedo source
        json_obj[render_graph::LAMBERT_MATERIAL_TYPE_NAME]["albedo_source"] = lambert_param_ptr->albedo_source;

        // Normal source
        json_obj[render_graph::LAMBERT_MATERIAL_TYPE_NAME]["normal_source"] = lambert_param_ptr->normal_source;

        // AO source
        json_obj[render_graph::LAMBERT_MATERIAL_TYPE_NAME]["ao_source"] = lambert_param_ptr->ao_source;

        // Emission source
        json_obj[render_graph::LAMBERT_MATERIAL_TYPE_NAME]["emission_source"] = lambert_param_ptr->emission_source;

        // Emission color
        json_obj[render_graph::LAMBERT_MATERIAL_TYPE_NAME]["emission_color"] = {
            lambert_param_ptr->emission_color.x,
            lambert_param_ptr->emission_color.y,
            lambert_param_ptr->emission_color.z,
            lambert_param_ptr->emission_color.w};

        // Albedo texture asset name
        {
            const asset_loader::Asset& albedo_texture_asset
                = asset_service_view_ptr->GetAsset(lambert_additional_param_ptr->GetAlbedoTextureAssetID());
            json_obj[render_graph::LAMBERT_MATERIAL_TYPE_NAME]["albedo_texture_asset_name"] = albedo_texture_asset.GetName();
        }

        // Normal texture asset name
        {
            const asset_loader::Asset& normal_texture_asset
                = asset_service_view_ptr->GetAsset(lambert_additional_param_ptr->GetNormalTextureAssetID());
            json_obj[render_graph::LAMBERT_MATERIAL_TYPE_NAME]["normal_texture_asset_name"] = normal_texture_asset.GetName();
        }

        // AO texture asset name
        {
            const asset_loader::Asset& ao_texture_asset
                = asset_service_view_ptr->GetAsset(lambert_additional_param_ptr->GetAOTextureAssetID());
            json_obj[render_graph::LAMBERT_MATERIAL_TYPE_NAME]["ao_texture_asset_name"] = ao_texture_asset.GetName();
        }

        // Emission texture asset name
        {
            const asset_loader::Asset& emission_texture_asset
                = asset_service_view_ptr->GetAsset(lambert_additional_param_ptr->GetEmissionTextureAssetID());
            json_obj[render_graph::LAMBERT_MATERIAL_TYPE_NAME]["emission_texture_asset_name"] = emission_texture_asset.GetName();
        }

        return json_obj;
    },
    [](
        const nlohmann::json& json_obj,
        mono_service::ServiceProxyManager& service_proxy_manager) -> bool
    {
        // // Get lambert material json object
        // const nlohmann::json& lambert_json_obj = json_obj[render_graph::LAMBERT_MATERIAL_TYPE_NAME];

        // // Get asset service proxy, graphics service proxy, entity archive service proxy
        // std::unique_ptr<mono_service::ServiceProxy> asset_service_proxy = nullptr;
        // std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy = nullptr;
        // service_proxy_manager.WithLock([&](mono_service::ServiceProxyManager& service_proxy_manager_inner)
        // {
        //     asset_service_proxy 
        //         = service_proxy_manager_inner.GetServiceProxy(mono_asset_service::AssetServiceHandle::ID()).Clone();

        //     graphics_service_proxy 
        //         = service_proxy_manager_inner.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID()).Clone();
        // });

        // // Create asset service view
        // std::unique_ptr<mono_service::ServiceView> asset_service_view = asset_service_proxy->CreateView();
        // mono_asset_service::AssetServiceView* asset_service_view_ptr
        //     = dynamic_cast<mono_asset_service::AssetServiceView*>(asset_service_view.get());
        // assert(asset_service_view_ptr != nullptr && "Asset service view is null!");

        // // Create graphics service command list
        // std::unique_ptr<mono_service::ServiceCommandList> graphics_command_list = graphics_service_proxy->CreateCommandList();
        // mono_graphics_service::GraphicsCommandList* graphics_command_list_ptr
        //     = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(graphics_command_list.get());
        // assert(graphics_command_list_ptr != nullptr && "Graphics command list is null!");

        // // Get material handle manager
        // render_graph::MaterialHandleManager& material_handle_manager
        //     = render_graph::MaterialHandleManager::GetInstance();

        // // Create lambert material setup parameter
        // std::unique_ptr<render_graph::LambertMaterial::SetupParam> lambert_material_param
        //     = std::make_unique<render_graph::LambertMaterial::SetupParam>();
        // lambert_material_param->debug_name_prefix 
        //     = utility_header::StringToWstring(lambert_json_obj["debug_name_prefix"].get<std::string>());
        // lambert_material_param->base_color = DirectX::XMFLOAT4(
        //     lambert_json_obj["base_color"][0].get<float>(),
        //     lambert_json_obj["base_color"][1].get<float>(),
        //     lambert_json_obj["base_color"][2].get<float>(),
        //     lambert_json_obj["base_color"][3].get<float>());
        // lambert_material_param->albedo_source = lambert_json_obj["albedo_source"].get<uint32_t>();
        // lambert_material_param->normal_source = lambert_json_obj["normal_source"].get<uint32_t>();
        // lambert_material_param->ao_source = lambert_json_obj["ao_source"].get<uint32_t>();
        // lambert_material_param->emission_source = lambert_json_obj["emission_source"].get<uint32_t>();
        // lambert_material_param->emission_color = DirectX::XMFLOAT4(
        //     lambert_json_obj["emission_color"][0].get<float>(),
        //     lambert_json_obj["emission_color"][1].get<float>(),
        //     lambert_json_obj["emission_color"][2].get<float>(),
        //     lambert_json_obj["emission_color"][3].get<float>());

        // // Get texture assets and set texture handles
        // {
        //     const asset_loader::Asset& albedo_texture_asset
        //         = asset_service_view_ptr->GetAsset(
        //             asset_service_view_ptr->GetAssetHandleIDByName(
        //                 lambert_json_obj["albedo_texture_asset_name"].get<std::string>()));
        //     const mono_asset_extension::TextureAsset* albedo_texture_asset_ptr
        //         = dynamic_cast<const mono_asset_extension::TextureAsset*>(&albedo_texture_asset);
        //     lambert_material_param->albedo_texture_handle = albedo_texture_asset_ptr->GetTextureHandle();
        // }
        // {
        //     const asset_loader::Asset& normal_texture_asset
        //         = asset_service_view_ptr->GetAsset(
        //             asset_service_view_ptr->GetAssetHandleIDByName(
        //                 lambert_json_obj["normal_texture_asset_name"].get<std::string>()));
        //     const mono_asset_extension::TextureAsset* normal_texture_asset_ptr
        //         = dynamic_cast<const mono_asset_extension::TextureAsset*>(&normal_texture_asset);
        //     lambert_material_param->normal_texture_handle = normal_texture_asset_ptr->GetTextureHandle();
        // }
        // {
        //     const asset_loader::Asset& ao_texture_asset
        //         = asset_service_view_ptr->GetAsset(
        //             asset_service_view_ptr->GetAssetHandleIDByName(
        //                 lambert_json_obj["ao_texture_asset_name"].get<std::string>()));
        //     const mono_asset_extension::TextureAsset* ao_texture_asset_ptr
        //         = dynamic_cast<const mono_asset_extension::TextureAsset*>(&ao_texture_asset);
        //     lambert_material_param->ao_texture_handle = ao_texture_asset_ptr->GetTextureHandle();
        // }
        // {
        //     const asset_loader::Asset& emission_texture_asset
        //         = asset_service_view_ptr->GetAsset(
        //             asset_service_view_ptr->GetAssetHandleIDByName(
        //                 lambert_json_obj["emission_texture_asset_name"].get<std::string>()));
        //     const mono_asset_extension::TextureAsset* emission_texture_asset_ptr
        //         = dynamic_cast<const mono_asset_extension::TextureAsset*>(&emission_texture_asset);
        //     lambert_material_param->emission_texture_handle = emission_texture_asset_ptr->GetTextureHandle();
        // }

        // // Generate new material handle key
        // render_graph::MaterialHandleKey new_key = render_graph::MaterialHandleKeyGenerator::GetInstance().Generate();

        // // Register material handle
        // material_handle_manager.RegisterMaterialHandle(new_key, 

        return true;
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
    },
    [](const material_editor::SetupParamWrapper* setup_param,
        mono_service::ServiceProxyManager& service_proxy_manager) -> nlohmann::json
    {
        // Cast to PhongMaterial::SetupParam
        const render_graph::PhongMaterial::SetupParam* phong_param_ptr
            = dynamic_cast<const render_graph::PhongMaterial::SetupParam*>(setup_param->GetSetupParam());
        assert(phong_param_ptr != nullptr && "Invalid setup param type for Phong material");

        // Cast to PhongMaterialAdditionalSetupParam
        const mono_entity_archive_extension::PhongMaterialAdditionalSetupParam* phong_additional_param_ptr
            = dynamic_cast<const mono_entity_archive_extension::PhongMaterialAdditionalSetupParam*>(
                setup_param->GetAdditionalParam());
        assert(phong_additional_param_ptr != nullptr && "Invalid additional setup param type for Phong material");

        // Get asset service proxy, graphics service proxy, entity archive service proxy
        std::unique_ptr<mono_service::ServiceProxy> asset_service_proxy = nullptr;
        service_proxy_manager.WithLock([&](mono_service::ServiceProxyManager& service_proxy_manager_inner)
        {
            asset_service_proxy 
                = service_proxy_manager_inner.GetServiceProxy(mono_asset_service::AssetServiceHandle::ID()).Clone();
        });

        // Create asset service view
        std::unique_ptr<mono_service::ServiceView> asset_service_view = asset_service_proxy->CreateView();
        mono_asset_service::AssetServiceView* asset_service_view_ptr
            = dynamic_cast<mono_asset_service::AssetServiceView*>(asset_service_view.get());
        assert(asset_service_view_ptr != nullptr && "Asset service view is null!");

        // Get material handle manager
        render_graph::MaterialHandleManager& material_handle_manager
            = render_graph::MaterialHandleManager::GetInstance();

        // Create JSON object
        nlohmann::json json_obj;

        // Debug name prefix
        json_obj[render_graph::PHONG_MATERIAL_TYPE_NAME]["debug_name_prefix"] = phong_param_ptr->debug_name_prefix;

        // Base color
        json_obj[render_graph::PHONG_MATERIAL_TYPE_NAME]["base_color"] = {
            phong_param_ptr->base_color.x,
            phong_param_ptr->base_color.y,
            phong_param_ptr->base_color.z,
            phong_param_ptr->base_color.w};

        // Albedo source
        json_obj[render_graph::PHONG_MATERIAL_TYPE_NAME]["albedo_source"] = phong_param_ptr->albedo_source;

        // Normal source
        json_obj[render_graph::PHONG_MATERIAL_TYPE_NAME]["normal_source"] = phong_param_ptr->normal_source;

        // AO source
        json_obj[render_graph::PHONG_MATERIAL_TYPE_NAME]["ao_source"] = phong_param_ptr->ao_source;

        // Specular source
        json_obj[render_graph::PHONG_MATERIAL_TYPE_NAME]["specular_source"] = phong_param_ptr->specular_source;

        // Roughness source
        json_obj[render_graph::PHONG_MATERIAL_TYPE_NAME]["roughness_source"] = phong_param_ptr->roughness_source;

        // Roughness value
        json_obj[render_graph::PHONG_MATERIAL_TYPE_NAME]["roughness_value"] = phong_param_ptr->roughness_value;

        // Metalness source
        json_obj[render_graph::PHONG_MATERIAL_TYPE_NAME]["metalness_source"] = phong_param_ptr->metalness_source;

        // Metalness value
        json_obj[render_graph::PHONG_MATERIAL_TYPE_NAME]["metalness_value"] = phong_param_ptr->metalness_value;

        // Emission source
        json_obj[render_graph::PHONG_MATERIAL_TYPE_NAME]["emission_source"] = phong_param_ptr->emission_source;

        // Emission color
        json_obj[render_graph::PHONG_MATERIAL_TYPE_NAME]["emission_color"] = {
            phong_param_ptr->emission_color.x,
            phong_param_ptr->emission_color.y,
            phong_param_ptr->emission_color.z,
            phong_param_ptr->emission_color.w};

        // Albedo texture asset name
        {
            const asset_loader::Asset& albedo_texture_asset
                = asset_service_view_ptr->GetAsset(phong_additional_param_ptr->GetAlbedoTextureAssetID());
            json_obj[render_graph::PHONG_MATERIAL_TYPE_NAME]["albedo_texture_asset_name"] = albedo_texture_asset.GetName();
        }

        // Normal texture asset name
        {
            const asset_loader::Asset& normal_texture_asset
                = asset_service_view_ptr->GetAsset(phong_additional_param_ptr->GetNormalTextureAssetID());
            json_obj[render_graph::PHONG_MATERIAL_TYPE_NAME]["normal_texture_asset_name"] = normal_texture_asset.GetName();
        }

        // AO texture asset name
        {
            const asset_loader::Asset& ao_texture_asset
                = asset_service_view_ptr->GetAsset(phong_additional_param_ptr->GetAOTextureAssetID());
            json_obj[render_graph::PHONG_MATERIAL_TYPE_NAME]["ao_texture_asset_name"] = ao_texture_asset.GetName();
        }

        // Specular texture asset name
        {
            const asset_loader::Asset& specular_texture_asset
                = asset_service_view_ptr->GetAsset(phong_additional_param_ptr->GetSpecularTextureAssetID());
            json_obj[render_graph::PHONG_MATERIAL_TYPE_NAME]["specular_texture_asset_name"] = specular_texture_asset.GetName();
        }

        // Roughness texture asset name
        {
            const asset_loader::Asset& roughness_texture_asset
                = asset_service_view_ptr->GetAsset(phong_additional_param_ptr->GetRoughnessTextureAssetID());
            json_obj[render_graph::PHONG_MATERIAL_TYPE_NAME]["roughness_texture_asset_name"] = roughness_texture_asset.GetName();
        }

        // Metalness texture asset name
        {
            const asset_loader::Asset& metalness_texture_asset
                = asset_service_view_ptr->GetAsset(phong_additional_param_ptr->GetMetalnessTextureAssetID());
            json_obj[render_graph::PHONG_MATERIAL_TYPE_NAME]["metalness_texture_asset_name"] = metalness_texture_asset.GetName();
        }

        // Emission texture asset name
        {
            const asset_loader::Asset& emission_texture_asset
                = asset_service_view_ptr->GetAsset(phong_additional_param_ptr->GetEmissionTextureAssetID());
            json_obj[render_graph::PHONG_MATERIAL_TYPE_NAME]["emission_texture_asset_name"] = emission_texture_asset.GetName();
        }

        return json_obj;
    },
    [](const nlohmann::json& json_obj,
        mono_service::ServiceProxyManager& service_proxy_manager) -> bool
    {
        return true;
    });

//MATERIAL_SETUP_PARAM_EDITOR_REGISTRAR_END//

} // namespace mono_forge_app_template