#pragma once

#include <cassert>

#include "ecs/include/entity.h"
#include "ecs/include/component.h"
#include "component_editor/include/component_reflection_info.h"

#include "mono_entity_archive_service/include/dll_config.h"
#include "mono_entity_archive_service/include/entity_archive_service.h"

namespace mono_entity_archive_service
{

// The service view class
class MONO_ENTITY_ARCHIVE_SERVICE_DLL EntityArchiveServiceView :
    public mono_service::ServiceView
{
public:
    EntityArchiveServiceView(const mono_service::ServiceAPI& service_api);
    virtual ~EntityArchiveServiceView() override = default;

    // Check if the setup param for an entity's component is editable
    bool CheckIsEditable(ecs::Entity entity, ecs::ComponentID component_id) const;

    // Get the field map for a component ID
    const component_editor::FieldMap& GetComponentFieldMap(
        ecs::ComponentID component_id) const;

    // Get a setup param field value for an entity's component with specified type
    const uint8_t* GetSetupParamField(
        ecs::Entity entity, ecs::ComponentID component_id, std::string_view field_name) const;

    // Get the setup param for an entity's component
    const ecs::Component::SetupParam& GetSetupParam(
        ecs::Entity entity, ecs::ComponentID component_id) const;

    // Get component name map
    const component_editor::ComponentNameMap& GetComponentNameMap() const;

    // Get component name map
    const component_editor::ComponentAdderMap& GetComponentAdderMap() const;

    // Get component setup param field type registry
    const ComponentSetupParamFieldTypeRegistry& GetSetupParamFieldTypeRegistry() const;

    // Check if a component is editable
    bool CheckComponentEditable(ecs::ComponentID component_id) const;

    // Get edited infos
    std::vector<component_editor::EditedInfo> GetEditedInfos() const;

    // Get setup param editor
    const component_editor::SetupParamEditor& GetSetupParamEditor() const;

    // Get material setup param for a material handle
    const material_editor::SetupParamWrapper* GetMaterialSetupParam(render_graph::MaterialHandle* material_handle) const;

    // Get material setup param edit function for a material handle
    const MaterialSetupParamEditFunc& GetMaterialSetupParamEditFunc(
        render_graph::MaterialTypeHandleID material_type_handle_id) const;

    // Get material setup param create function for a material handle
    const MaterialSetupParamCreateFunc& GetMaterialSetupParamCreateFunc(
        render_graph::MaterialTypeHandleID material_type_handle_id) const;

    // Get the material setup param editor registry
    const MaterialSetupParamEditorRegistry& GetMaterialSetupParamEditorRegistry() const;
};

} // namespace mono_entity_archive_service