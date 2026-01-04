#pragma once

#include <any>

#include "mono_service/include/service_command.h"

#include "mono_entity_archive_service/include/dll_config.h"
#include "mono_entity_archive_service/include/entity_archive_service.h"

namespace mono_entity_archive_service
{

// The graphics command list class
class MONO_ENTITY_ARCHIVE_SERVICE_DLL EntityArchiveServiceCommandList :
    public mono_service::ServiceCommandList
{
public:
    EntityArchiveServiceCommandList() = default;
    ~EntityArchiveServiceCommandList() override = default;

    // Add setup param for an entity's component to the entity archive service
    void AddSetupParam(
        ecs::Entity entity, ecs::ComponentID component_id, std::unique_ptr<ecs::Component::SetupParam> setup_param);

    // Edit setup param field for an entity's component in the entity archive service
    void EditSetupParamField(
        ecs::Entity entity, ecs::ComponentID component_id, std::string_view field_name, std::any new_value);

    // Edit entire setup param for an entity's component in the entity archive service
    void EditSetupParam(
        ecs::Entity entity, ecs::ComponentID component_id, std::unique_ptr<ecs::Component::SetupParam> new_setup_param);

    // Erase setup param for an entity's component from the entity archive service
    void EraseSetupParam(ecs::Entity entity, ecs::ComponentID component_id);

    // Erase all setup params for an entity from the entity archive service
    void EraseAllSetupParam(ecs::Entity entity);

    // Add setup param for a material to the entity archive service
    void AddSetupParam(
        render_graph::MaterialHandle* material_handle, std::unique_ptr<material_editor::SetupParamWrapper> setup_param);

    // Edit setup param field for a material in the entity archive service
    void EditSetupParam(
        render_graph::MaterialHandle* material_handle, std::unique_ptr<material_editor::SetupParamWrapper> new_setup_param);

    // Erase setup param for a material from the entity archive service
    void EraseSetupParam(render_graph::MaterialHandle material_handle);
};

} // namespace mono_entity_archive_service