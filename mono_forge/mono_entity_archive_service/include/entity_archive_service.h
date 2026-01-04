#pragma once
#include "mono_service/include/service.h"

#include "component_editor/include/component_reflection_registry.h"
#include "component_editor/include/component_adder.h"
#include "component_editor/include/setup_param_manager.h"
#include "component_editor/include/setup_param_editor.h"
#include "component_editor/include/setup_param_field_type.h"

#include "material_editor/include/setup_param_manager.h"
#include "material_editor/include/setup_param_editor_registry.h"

#include "mono_service/include/service_registry.h"
#include "mono_service/include/service_proxy_manager.h"
#include "mono_entity_archive_service/include/dll_config.h"

namespace mono_entity_archive_service
{

// Alias for setup param field type registry with ServiceProxyManager reference
using ComponentSetupParamFieldTypeRegistry = component_editor::SetupParamFieldTypeRegistry<mono_service::ServiceProxyManager&>;

// Alias for setup param field type registrar with ServiceProxyManager reference
using ComponentSetupParamFieldTypeRegistrar = component_editor::SetupParamFieldTypeRegistrar<mono_service::ServiceProxyManager&>;

// Alias for setup param any field create function with ServiceProxyManager reference
using ComponentSetupParamAnyFieldCreateFunc = component_editor::SetupParamAnyFieldCreateFunc<mono_service::ServiceProxyManager&>;

// Alias for setup param any field edit function with ServiceProxyManager reference
using ComponentSetupParamAnyFieldEditFunc = component_editor::SetupParamAnyFieldEditFunc<mono_service::ServiceProxyManager&>;

// Alias for material setup param editor registry with ServiceProxyManager reference
using MaterialSetupParamEditorRegistry = material_editor::SetupParamEditorRegistry<mono_service::ServiceProxyManager&>;

// Alias for material setup param edit function with ServiceProxyManager reference
using MaterialSetupParamEditFunc = material_editor::SetupParamEditFunc<mono_service::ServiceProxyManager&>;

// Alias for material setup param create function with ServiceProxyManager reference
using MaterialSetupParamCreateFunc = material_editor::MaterialCreateFunc<mono_service::ServiceProxyManager&>;

// Alias for material setup param editor registrar with ServiceProxyManager reference
using MaterialSetupParamEditorRegistrar = material_editor::SetupParamEditorRegistrar<mono_service::ServiceProxyManager&>;

// The entity_archive_service API
// It provides access to entity_archive_service internals for commands
class MONO_ENTITY_ARCHIVE_SERVICE_DLL EntityArchiveServiceAPI :
    public mono_service::ServiceAPI
{
public:
    EntityArchiveServiceAPI() = default;
    virtual ~EntityArchiveServiceAPI() = default;

    // Get the setup param adder
    virtual component_editor::SetupParamAdder& GetSetupParamAdder() = 0;

    // Get the setup param eraser
    virtual component_editor::SetupParamEraser& GetSetupParamEraser() = 0;

    // Get the setup param editor
    virtual component_editor::SetupParamEditor& GetSetupParamEditor() = 0;
    
    // Get the material setup param manager
    virtual const material_editor::SetupParamManager& GetMaterialSetupParamManager() const = 0;

    // Get the material setup param adder
    virtual material_editor::SetupParamAdder& GetMaterialSetupParamAdder() = 0;

    // Get the material setup param eraser
    virtual material_editor::SetupParamEraser& GetMaterialSetupParamEraser() = 0;

    // Check if the setup param for an entity's component is editable
    virtual bool CheckIsEditable(ecs::Entity entity, ecs::ComponentID component_id) const = 0;

    // Get the field map for a component ID
    virtual utility_header::ConstSharedLockedValue<component_editor::FieldMap> GetComponentFieldMap(
        ecs::ComponentID component_id) const = 0;

    // Get a setup param field value for an entity's component
    virtual const uint8_t* GetSetupParamField(
        ecs::Entity entity, ecs::ComponentID component_id, std::string_view field_name) const = 0;

    // Get the setup param for an entity's component
    virtual utility_header::ConstSharedLockedValue<ecs::Component::SetupParam> GetSetupParam(
        ecs::Entity entity, ecs::ComponentID component_id) const = 0;

    // Replace the setup param for an entity's component
    virtual void ReplaceSetupParam(
        ecs::Entity entity, ecs::ComponentID component_id, std::unique_ptr<ecs::Component::SetupParam> new_setup_param) = 0;

    // Get the component name map
    virtual utility_header::ConstSharedLockedValue<component_editor::ComponentNameMap> GetComponentNameMap() const = 0;

    // Get the component adder map
    virtual utility_header::ConstSharedLockedValue<component_editor::ComponentAdderMap> GetComponentAdderMap() const = 0;

    // Get the component setup param type registry
    virtual utility_header::ConstSharedLockedValue<ComponentSetupParamFieldTypeRegistry> GetSetupParamFieldTypeRegistry() const = 0;

    // Check if a component is editable
    virtual bool CheckComponentEditable(ecs::ComponentID component_id) const = 0;

    // Get the list of edited setup components and entities
    virtual std::vector<component_editor::EditedInfo> GetEditedInfos() const = 0;

    // Get the material setup param editor registry
    virtual utility_header::ConstSharedLockedValue<MaterialSetupParamEditorRegistry> GetMaterialSetupParamEditorRegistry() const = 0;

    // Replace the setup param for a material
    virtual void ReplaceSetupParam(
        render_graph::MaterialHandle* material_handle, std::unique_ptr<material_editor::SetupParamWrapper> new_setup_param) = 0;
};

// The number of command queue buffers for entity_archive_service
constexpr size_t SERVICE_COMMAND_QUEUE_BUFFER_COUNT = 2;

// The transform service handle type
class MONO_ENTITY_ARCHIVE_SERVICE_DLL EntityArchiveServiceHandle : public mono_service::ServiceHandle<EntityArchiveServiceHandle> {};

// The entity_archive_service class
class MONO_ENTITY_ARCHIVE_SERVICE_DLL EntityArchiveService :
    public mono_service::Service,
    private EntityArchiveServiceAPI
{
public:
    EntityArchiveService(mono_service::ServiceThreadAffinityID thread_affinity_id);
    virtual ~EntityArchiveService();

    /*******************************************************************************************************************
     * Service
    /******************************************************************************************************************/

    class SetupParam :
        public mono_service::Service::SetupParam
    {
    public:
        SetupParam() :
            mono_service::Service::SetupParam(SERVICE_COMMAND_QUEUE_BUFFER_COUNT)
        {
        }

        // Component name map
        component_editor::ComponentNameMap component_name_map;

        // Component adder map
        component_editor::ComponentAdderMap component_adder_map;

        // Component reflection registry
        component_editor::ComponentReflectionRegistry component_reflection_registry;

        // Entity Setup param field type registry
        ComponentSetupParamFieldTypeRegistry setup_param_field_type_registry_;

        // Entity Setup param field value setter
        component_editor::SetupParamFieldValueSetter setup_param_field_value_setter;

        // Material setup param editor registry
        MaterialSetupParamEditorRegistry material_setup_param_editor_registry;

        virtual ~SetupParam() override = default;
    };
    virtual bool Setup(mono_service::Service::SetupParam& param) override;
    virtual bool PreUpdate() override;
    virtual bool Update() override;
    virtual bool PostUpdate() override;
    virtual std::unique_ptr<mono_service::ServiceCommandList> CreateCommandList() override;
    virtual std::unique_ptr<mono_service::ServiceView> CreateView() override;

protected:
    /*******************************************************************************************************************
     * EntityArchiveService API
    /******************************************************************************************************************/

    EntityArchiveServiceAPI& GetAPI() { return *this; }
    const EntityArchiveServiceAPI& GetAPI() const { return *this; }

    virtual component_editor::SetupParamAdder& GetSetupParamAdder() override;
    virtual component_editor::SetupParamEraser& GetSetupParamEraser() override;
    virtual component_editor::SetupParamEditor& GetSetupParamEditor() override;

    virtual const material_editor::SetupParamManager& GetMaterialSetupParamManager() const override;
    virtual material_editor::SetupParamAdder& GetMaterialSetupParamAdder() override;
    virtual material_editor::SetupParamEraser& GetMaterialSetupParamEraser() override;

    virtual bool CheckIsEditable(ecs::Entity entity, ecs::ComponentID component_id) const override;

    virtual utility_header::ConstSharedLockedValue<component_editor::FieldMap> GetComponentFieldMap(
        ecs::ComponentID component_id) const override;
    virtual const uint8_t* GetSetupParamField(
        ecs::Entity entity, ecs::ComponentID component_id, std::string_view field_name) const override;
    virtual utility_header::ConstSharedLockedValue<ecs::Component::SetupParam> GetSetupParam(
        ecs::Entity entity, ecs::ComponentID component_id) const override;
    
    virtual void ReplaceSetupParam(
        ecs::Entity entity, ecs::ComponentID component_id, std::unique_ptr<ecs::Component::SetupParam> new_setup_param) override;

    virtual utility_header::ConstSharedLockedValue<component_editor::ComponentNameMap> GetComponentNameMap() const override;
    virtual utility_header::ConstSharedLockedValue<component_editor::ComponentAdderMap> GetComponentAdderMap() const override;
    virtual utility_header::ConstSharedLockedValue<ComponentSetupParamFieldTypeRegistry> GetSetupParamFieldTypeRegistry() const override;
    virtual bool CheckComponentEditable(ecs::ComponentID component_id) const override;
    virtual std::vector<component_editor::EditedInfo> GetEditedInfos() const override;

    virtual utility_header::ConstSharedLockedValue<MaterialSetupParamEditorRegistry> GetMaterialSetupParamEditorRegistry() const override;
    virtual void ReplaceSetupParam(
        render_graph::MaterialHandle* material_handle, 
        std::unique_ptr<material_editor::SetupParamWrapper> new_setup_param) override;

private:
    // Component name map
    component_editor::ComponentNameMap component_name_map_;
    
    // Component adder map
    component_editor::ComponentAdderMap component_adder_map_;

    // Component reflection registry
    component_editor::ComponentReflectionRegistry component_reflection_registry_;

    // Setup param field type registry
    ComponentSetupParamFieldTypeRegistry setup_param_field_type_registry_;

    // Entity setup param map
    component_editor::EntitySetupParamMap entity_setup_param_map_;

    // Entity Setup param manager
    std::unique_ptr<component_editor::SetupParamManager> entity_setup_param_manager_ = nullptr;

    // Entity Setup param adder
    std::unique_ptr<component_editor::SetupParamAdder> entity_setup_param_adder_ = nullptr;

    // Entity Setup param eraser
    std::unique_ptr<component_editor::SetupParamEraser> entity_setup_param_eraser_ = nullptr;

    // Entity Setup param editor
    std::unique_ptr<component_editor::SetupParamEditor> entity_setup_param_editor_ = nullptr;

    // Material setup param map
    material_editor::SetupParamMap material_setup_param_map_;

    // Material Setup param manager
    std::unique_ptr<material_editor::SetupParamManager> material_setup_param_manager_ = nullptr;

    // Material Setup param adder
    std::unique_ptr<material_editor::SetupParamAdder> material_setup_param_adder_ = nullptr;

    // Material Setup param eraser
    std::unique_ptr<material_editor::SetupParamEraser> material_setup_param_eraser_ = nullptr;

    // Material Setup param editor registry
    MaterialSetupParamEditorRegistry material_setup_param_editor_registry_;
};

// Helper function to add setup param to entity archive service
MONO_ENTITY_ARCHIVE_SERVICE_DLL void AddSetupParam(
    mono_service::ServiceProxyManager& service_proxy_manager,
    ecs::Entity entity, ecs::ComponentID component_id, std::unique_ptr<ecs::Component::SetupParam> setup_param);

// Helper function to add setup param to entity archive service
MONO_ENTITY_ARCHIVE_SERVICE_DLL void AddSetupParam(
    mono_service::ServiceProxy &entity_archive_service_proxy,
    ecs::Entity entity, ecs::ComponentID component_id, std::unique_ptr<ecs::Component::SetupParam> setup_param);

} // namespace mono_entity_archive_service