#include "mono_entity_archive_extension/src/pch.h"
#include "mono_entity_archive_extension/include/material_editor_system.h"

#include "ecs/include/world.h"
#include "mono_meta_extension/include/meta_component.h"
#include "mono_graphics_extension/include/ui_component.h"
#include "mono_entity_archive_extension/include/material_editor_ui_component.h"
#include "mono_entity_archive_extension/include/material_editor_drawer.h"

#include "mono_entity_archive_service/include/entity_archive_service_command_list.h"
#include "mono_entity_archive_service/include/entity_archive_service_view.h"
#include "mono_graphics_service/include/graphics_command_list.h"

namespace mono_entity_archive_extension
{

MaterialEditorSystem::MaterialEditorSystem(
    std::unique_ptr<mono_service::ServiceProxy> entity_archive_service_proxy,
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy,
    mono_service::ServiceProxyManager& service_proxy_manager) :
    entity_archive_service_proxy_(std::move(entity_archive_service_proxy)),
    graphics_service_proxy_(std::move(graphics_service_proxy)),
    service_proxy_manager_(service_proxy_manager)
{
    assert(entity_archive_service_proxy_ != nullptr && "Entity archive service proxy is null!");
    assert(graphics_service_proxy_ != nullptr && "Graphics service proxy is null!");
}

MaterialEditorSystem::~MaterialEditorSystem()
{
}

bool MaterialEditorSystem::PreUpdate(ecs::World& world)
{
    return true; // Success
}

bool MaterialEditorSystem::Update(ecs::World& world)
{
    for (const ecs::Entity& entity : world.View(MaterialEditorUIComponentHandle::ID())())
    {
        // Get meta component
        mono_meta_extension::MetaComponent* meta_component =
            world.GetComponent<mono_meta_extension::MetaComponent>(entity, mono_meta_extension::MetaComponentHandle::ID());
        assert(meta_component != nullptr && "MetaComponent is null!");

        if (!meta_component->IsActiveSelf())
            continue; // Skip inactive entities

        // Get ui component
        mono_graphics_extension::UIComponent* ui_component =
            world.GetComponent<mono_graphics_extension::UIComponent>(entity, mono_graphics_extension::UIComponentHandle::ID());
        assert(ui_component != nullptr && "UIComponent is null!");

        // Get material editor drawer
        MaterialEditorDrawer* material_editor_drawer
            = dynamic_cast<MaterialEditorDrawer*>(&ui_component->GetDrawer());
        assert(material_editor_drawer != nullptr && "MaterialEditorDrawer is null!");

        // Take edited material setup params
        std::vector<MaterialEditorDrawer::EditedMaterialInfo> edited_material_infos
            = material_editor_drawer->TakeEditedMaterialInfos();

        // Create entity archive service command list
        std::unique_ptr<mono_service::ServiceCommandList> entity_archive_command_list = entity_archive_service_proxy_->CreateCommandList();
        mono_entity_archive_service::EntityArchiveServiceCommandList* entity_archive_command_list_ptr
            = dynamic_cast<mono_entity_archive_service::EntityArchiveServiceCommandList*>(entity_archive_command_list.get());
        assert(entity_archive_command_list_ptr != nullptr && "Entity archive service command list is null!");

        // Create entity archive service view
        std::unique_ptr<mono_service::ServiceView> entity_archive_view = entity_archive_service_proxy_->CreateView();
        mono_entity_archive_service::EntityArchiveServiceView* entity_archive_service_view_ptr
            = dynamic_cast<mono_entity_archive_service::EntityArchiveServiceView*>(entity_archive_view.get());
        assert(entity_archive_service_view_ptr != nullptr && "Entity archive service view is null!");

        // Create graphics service command list
        std::unique_ptr<mono_service::ServiceCommandList> graphics_command_list = graphics_service_proxy_->CreateCommandList();
        mono_graphics_service::GraphicsCommandList* graphics_command_list_ptr
            = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(graphics_command_list.get());
        assert(graphics_command_list_ptr != nullptr && "Graphics service command list is null!");

        // Iterate through edited material infos
        for (MaterialEditorDrawer::EditedMaterialInfo& edited_info : edited_material_infos)
        {
            // Update material setup param
            graphics_command_list_ptr->UpdateMaterialBuffer(
                edited_info.material_handle, std::move(edited_info.setup_param->GetSetupParam()->Clone()));

            // Edit material setup param in entity archive service
            entity_archive_command_list_ptr->EditSetupParam(
                edited_info.material_handle, std::move(edited_info.setup_param));
        }

        // Get material create request
        MaterialEditorDrawer::MaterialCreateRequest create_request = material_editor_drawer->GetCreateMaterialRequested();

        // Check if create material is requested
        if (create_request.requested)
        {
            // Get material setup param create function from entity archive service view
            const mono_entity_archive_service::MaterialSetupParamCreateFunc& setup_param_create_func
                = entity_archive_service_view_ptr->GetMaterialSetupParamCreateFunc(create_request.material_type_handle_id);
            assert(setup_param_create_func && "Material setup param create function is null!");

            // Create new material
            bool created = setup_param_create_func(service_proxy_manager_);
            assert(created && "Failed to create new material!");
        }

        // Get material delete request
        MaterialEditorDrawer::MaterialDeleteRequest delete_request = material_editor_drawer->GetDeleteMaterialRequested();

        // Check if delete material is requested
        if (delete_request.requested)
        {
            // Get material handle
            render_graph::MaterialHandle* material_handle
                = render_graph::MaterialHandleManager::GetInstance().GetMaterialHandle(delete_request.material_handle_key);

            // Delete material setup param
            entity_archive_command_list_ptr->EraseSetupParam(*material_handle);

            // Destroy material in graphics service
            graphics_command_list_ptr->DestroyMaterial(material_handle);

            // Unregister material handle
            render_graph::MaterialHandleManager::GetInstance().UnregisterMaterialHandle(delete_request.material_handle_key);
        }

        // Submit entity archive service command list
        entity_archive_service_proxy_->SubmitCommandList(std::move(entity_archive_command_list));

        // Submit graphics service command list
        graphics_service_proxy_->SubmitCommandList(std::move(graphics_command_list));
    }

    return true; // Success
}

bool MaterialEditorSystem::PostUpdate(ecs::World& world)
{
    return true; // Success
}

ecs::SystemID MaterialEditorSystem::GetID() const
{
    return MaterialEditorSystemHandle::ID();
}

} // namespace mono_entity_archive_extension