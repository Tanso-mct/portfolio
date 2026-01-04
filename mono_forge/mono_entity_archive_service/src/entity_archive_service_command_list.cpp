#include "mono_entity_archive_service/src/pch.h"
#include "mono_entity_archive_service/include/entity_archive_service_command_list.h"

namespace mono_entity_archive_service
{

void EntityArchiveServiceCommandList::AddSetupParam(
    ecs::Entity entity, ecs::ComponentID component_id, std::unique_ptr<ecs::Component::SetupParam> setup_param)
{
    AddCommand([entity, component_id, setup_param = std::move(setup_param)](mono_service::ServiceAPI& api) mutable
    {
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, EntityArchiveServiceAPI>::value,
            "EntityArchiveServiceAPI must be derived from ServiceAPI.");
        EntityArchiveServiceAPI& entity_archive_service_api = dynamic_cast<EntityArchiveServiceAPI&>(api);

        // Add setup param to setup param adder
        entity_archive_service_api.GetSetupParamAdder().AddSetupParam(entity, component_id, std::move(setup_param));

        return true; // Success
    });
}

void EntityArchiveServiceCommandList::EditSetupParamField(
    ecs::Entity entity, ecs::ComponentID component_id, std::string_view field_name, std::any new_value)
{
    AddCommand([entity, component_id, field_name, new_value](mono_service::ServiceAPI& api)
    {
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, EntityArchiveServiceAPI>::value,
            "EntityArchiveServiceAPI must be derived from ServiceAPI.");
        EntityArchiveServiceAPI& entity_archive_service_api = dynamic_cast<EntityArchiveServiceAPI&>(api);

        // Edit setup param field in setup param editor
        entity_archive_service_api.GetSetupParamEditor().Edit(entity, component_id, field_name, new_value);

        return true; // Success
    });
}

void EntityArchiveServiceCommandList::EditSetupParam(
    ecs::Entity entity, ecs::ComponentID component_id, std::unique_ptr<ecs::Component::SetupParam> new_setup_param)
{
    AddCommand([entity, component_id, new_setup_param = std::move(new_setup_param)](mono_service::ServiceAPI& api) mutable
    {
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, EntityArchiveServiceAPI>::value,
            "EntityArchiveServiceAPI must be derived from ServiceAPI.");
        EntityArchiveServiceAPI& entity_archive_service_api = dynamic_cast<EntityArchiveServiceAPI&>(api);

        // Replace setup param in entity archive service API
        entity_archive_service_api.ReplaceSetupParam(entity, component_id, std::move(new_setup_param));

        return true; // Success
    });
}

void EntityArchiveServiceCommandList::EraseSetupParam(ecs::Entity entity, ecs::ComponentID component_id)
{
    AddCommand([entity, component_id](mono_service::ServiceAPI& api)
    {
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, EntityArchiveServiceAPI>::value,
            "EntityArchiveServiceAPI must be derived from ServiceAPI.");
        EntityArchiveServiceAPI& entity_archive_service_api = dynamic_cast<EntityArchiveServiceAPI&>(api);

        // Erase setup param from setup param eraser
        entity_archive_service_api.GetSetupParamEraser().EraseSetupParam(entity, component_id);

        return true; // Success
    });
}

void EntityArchiveServiceCommandList::EraseAllSetupParam(ecs::Entity entity)
{
    AddCommand([entity](mono_service::ServiceAPI& api)
    {
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, EntityArchiveServiceAPI>::value,
            "EntityArchiveServiceAPI must be derived from ServiceAPI.");
        EntityArchiveServiceAPI& entity_archive_service_api = dynamic_cast<EntityArchiveServiceAPI&>(api);

        // Erase all setup params for the entity from setup param eraser
        entity_archive_service_api.GetSetupParamEraser().EraseSetupParam(entity);

        return true; // Success
    });
}

void EntityArchiveServiceCommandList::AddSetupParam(
    render_graph::MaterialHandle* material_handle, std::unique_ptr<material_editor::SetupParamWrapper> setup_param)
{
    AddCommand([material_handle, setup_param = std::move(setup_param)](mono_service::ServiceAPI& api) mutable
    {
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, EntityArchiveServiceAPI>::value,
            "EntityArchiveServiceAPI must be derived from ServiceAPI.");
        EntityArchiveServiceAPI& entity_archive_service_api = dynamic_cast<EntityArchiveServiceAPI&>(api);

        // Add setup param to material setup param manager
        entity_archive_service_api.GetMaterialSetupParamAdder().AddSetupParam(
            material_handle, std::move(setup_param));

        return true; // Success
    });
}

void EntityArchiveServiceCommandList::EditSetupParam(
    render_graph::MaterialHandle* material_handle, std::unique_ptr<material_editor::SetupParamWrapper> new_setup_param)
{
    AddCommand([material_handle, new_setup_param = std::move(new_setup_param)](mono_service::ServiceAPI& api) mutable
    {
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, EntityArchiveServiceAPI>::value,
            "EntityArchiveServiceAPI must be derived from ServiceAPI.");
        EntityArchiveServiceAPI& entity_archive_service_api = dynamic_cast<EntityArchiveServiceAPI&>(api);

        // Replace setup param in material setup param manager
        entity_archive_service_api.ReplaceSetupParam(material_handle, std::move(new_setup_param));

        return true; // Success
    });
}

void EntityArchiveServiceCommandList::EraseSetupParam(render_graph::MaterialHandle material_handle)
{
    AddCommand([material_handle](mono_service::ServiceAPI& api)
    {
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, EntityArchiveServiceAPI>::value,
            "EntityArchiveServiceAPI must be derived from ServiceAPI.");
        EntityArchiveServiceAPI& entity_archive_service_api = dynamic_cast<EntityArchiveServiceAPI&>(api);

        // Erase setup param from material setup param eraser
        entity_archive_service_api.GetMaterialSetupParamEraser().EraseSetupParam(material_handle);

        return true; // Success
    });
}

} // namespace mono_entity_archive_service