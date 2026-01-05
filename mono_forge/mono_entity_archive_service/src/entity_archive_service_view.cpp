#include "mono_service/src/pch.h"
#include "mono_entity_archive_service/include/entity_archive_service_view.h"

namespace mono_entity_archive_service
{

EntityArchiveServiceView::EntityArchiveServiceView(const mono_service::ServiceAPI &service_api) :
    mono_service::ServiceView(service_api)
{
}

bool EntityArchiveServiceView::CheckIsEditable(ecs::Entity entity, ecs::ComponentID component_id) const
{
    // Get entity archive service API
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, EntityArchiveServiceAPI>::value,
        "EntityArchiveServiceAPI must be derived from ServiceAPI.");
    const EntityArchiveServiceAPI& entity_archive_service_api = 
        dynamic_cast<const EntityArchiveServiceAPI&>(service_api_);

    return entity_archive_service_api.CheckIsEditable(entity, component_id);
}

const component_editor::FieldMap& EntityArchiveServiceView::GetComponentFieldMap(
    ecs::ComponentID component_id) const
{
    // Get entity archive service API
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, EntityArchiveServiceAPI>::value,
        "EntityArchiveServiceAPI must be derived from ServiceAPI.");
    const EntityArchiveServiceAPI& entity_archive_service_api = 
        dynamic_cast<const EntityArchiveServiceAPI&>(service_api_);

    return entity_archive_service_api.GetComponentFieldMap(component_id);
}

const uint8_t* EntityArchiveServiceView::GetSetupParamField(
    ecs::Entity entity, ecs::ComponentID component_id, std::string_view field_name) const
{
    // Get entity archive service API
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, EntityArchiveServiceAPI>::value,
        "EntityArchiveServiceAPI must be derived from ServiceAPI.");
    const EntityArchiveServiceAPI& entity_archive_service_api = 
        dynamic_cast<const EntityArchiveServiceAPI&>(service_api_);

    return entity_archive_service_api.GetSetupParamField(entity, component_id, field_name);
}

const ecs::Component::SetupParam& EntityArchiveServiceView::GetSetupParam(
    ecs::Entity entity, ecs::ComponentID component_id) const
{
    // Get entity archive service API
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, EntityArchiveServiceAPI>::value,
        "EntityArchiveServiceAPI must be derived from ServiceAPI.");
    const EntityArchiveServiceAPI& entity_archive_service_api = 
        dynamic_cast<const EntityArchiveServiceAPI&>(service_api_);

    return entity_archive_service_api.GetSetupParam(entity, component_id);
}

const component_editor::ComponentNameMap& EntityArchiveServiceView::GetComponentNameMap() const
{
    // Get entity archive service API
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, EntityArchiveServiceAPI>::value,
        "EntityArchiveServiceAPI must be derived from ServiceAPI.");
    const EntityArchiveServiceAPI& entity_archive_service_api = 
        dynamic_cast<const EntityArchiveServiceAPI&>(service_api_);

    return entity_archive_service_api.GetComponentNameMap();
}

const component_editor::ComponentAdderMap& EntityArchiveServiceView::GetComponentAdderMap() const
{
    // Get entity archive service API
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, EntityArchiveServiceAPI>::value,
        "EntityArchiveServiceAPI must be derived from ServiceAPI.");
    const EntityArchiveServiceAPI& entity_archive_service_api = 
        dynamic_cast<const EntityArchiveServiceAPI&>(service_api_);

    return entity_archive_service_api.GetComponentAdderMap();
}

const ComponentSetupParamFieldTypeRegistry& EntityArchiveServiceView::GetSetupParamFieldTypeRegistry() const
{
    // Get entity archive service API
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, EntityArchiveServiceAPI>::value,
        "EntityArchiveServiceAPI must be derived from ServiceAPI.");
    const EntityArchiveServiceAPI& entity_archive_service_api = 
        dynamic_cast<const EntityArchiveServiceAPI&>(service_api_);

    return entity_archive_service_api.GetSetupParamFieldTypeRegistry();
}

bool EntityArchiveServiceView::CheckComponentEditable(ecs::ComponentID component_id) const
{
    // Get entity archive service API
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, EntityArchiveServiceAPI>::value,
        "EntityArchiveServiceAPI must be derived from ServiceAPI.");
    const EntityArchiveServiceAPI& entity_archive_service_api = 
        dynamic_cast<const EntityArchiveServiceAPI&>(service_api_);

    return entity_archive_service_api.CheckComponentEditable(component_id);
}

std::vector<component_editor::EditedInfo> EntityArchiveServiceView::GetEditedInfos() const
{
    // Get entity archive service API
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, EntityArchiveServiceAPI>::value,
        "EntityArchiveServiceAPI must be derived from ServiceAPI.");
    const EntityArchiveServiceAPI& entity_archive_service_api = 
        dynamic_cast<const EntityArchiveServiceAPI&>(service_api_);

    return entity_archive_service_api.GetEditedInfos();
}

const component_editor::SetupParamEditor& EntityArchiveServiceView::GetSetupParamEditor() const
{
    // Get entity archive service API
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, EntityArchiveServiceAPI>::value,
        "EntityArchiveServiceAPI must be derived from ServiceAPI.");
    const EntityArchiveServiceAPI& entity_archive_service_api = 
        dynamic_cast<const EntityArchiveServiceAPI&>(service_api_);

    return entity_archive_service_api.GetSetupParamEditor();
}

const material_editor::SetupParamWrapper* EntityArchiveServiceView::GetMaterialSetupParam(
    render_graph::MaterialHandle* material_handle) const
{
    // Get entity archive service API
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, EntityArchiveServiceAPI>::value,
        "EntityArchiveServiceAPI must be derived from ServiceAPI.");
    const EntityArchiveServiceAPI& entity_archive_service_api = 
        dynamic_cast<const EntityArchiveServiceAPI&>(service_api_);

    return entity_archive_service_api.GetMaterialSetupParamManager().GetSetupParam(material_handle);
}

const MaterialSetupParamEditFunc& EntityArchiveServiceView::GetMaterialSetupParamEditFunc(
    render_graph::MaterialTypeHandleID material_type_handle_id) const
{
    // Get entity archive service API
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, EntityArchiveServiceAPI>::value,
        "EntityArchiveServiceAPI must be derived from ServiceAPI.");
    const EntityArchiveServiceAPI& entity_archive_service_api = 
        dynamic_cast<const EntityArchiveServiceAPI&>(service_api_);

    return entity_archive_service_api.GetMaterialSetupParamEditorRegistry().GetSetupParamEditor(material_type_handle_id);
}

const MaterialSetupParamCreateFunc& EntityArchiveServiceView::GetMaterialSetupParamCreateFunc(
    render_graph::MaterialTypeHandleID material_type_handle_id) const
{
    // Get entity archive service API
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, EntityArchiveServiceAPI>::value,
        "EntityArchiveServiceAPI must be derived from ServiceAPI.");
    const EntityArchiveServiceAPI& entity_archive_service_api = 
        dynamic_cast<const EntityArchiveServiceAPI&>(service_api_);

    return entity_archive_service_api.GetMaterialSetupParamEditorRegistry().GetSetupParamCreator(material_type_handle_id);
}

const MaterialSetupParamEditorRegistry&
    EntityArchiveServiceView::GetMaterialSetupParamEditorRegistry() const
{
    // Get entity archive service API
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, EntityArchiveServiceAPI>::value,
        "EntityArchiveServiceAPI must be derived from ServiceAPI.");
    const EntityArchiveServiceAPI& entity_archive_service_api = 
        dynamic_cast<const EntityArchiveServiceAPI&>(service_api_);

    return entity_archive_service_api.GetMaterialSetupParamEditorRegistry();
}

} // namespace mono_entity_archive_service