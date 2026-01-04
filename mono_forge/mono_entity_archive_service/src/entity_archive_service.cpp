#include "mono_entity_archive_service/src/pch.h"
#include "mono_entity_archive_service/include/entity_archive_service.h"

#include "mono_entity_archive_service/include/entity_archive_service_command_list.h"
#include "mono_entity_archive_service/include/entity_archive_service_view.h"

namespace mono_entity_archive_service
{

EntityArchiveService::EntityArchiveService(mono_service::ServiceThreadAffinityID thread_affinity_id) :
    mono_service::Service(thread_affinity_id)
{
}

EntityArchiveService::~EntityArchiveService()
{
    
}

bool EntityArchiveService::Setup(mono_service::Service::SetupParam& param)
{
    // Call base class Setup
    if (!mono_service::Service::Setup(param))
        return false;

    // Cast to EntityArchiveService::SetupParam
    SetupParam* entity_archive_setup_param = dynamic_cast<SetupParam*>(&param); 
    assert(entity_archive_setup_param != nullptr && "Invalid setup param type for EntityArchiveService");

    // Move component name map
    component_name_map_ = std::move(entity_archive_setup_param->component_name_map);

    // Move component adder map
    component_adder_map_ = std::move(entity_archive_setup_param->component_adder_map);

    // Copy component reflection registry
    component_reflection_registry_ = entity_archive_setup_param->component_reflection_registry;

    // Move setup param field type registry
    setup_param_field_type_registry_ = std::move(entity_archive_setup_param->setup_param_field_type_registry_);

    // Move material setup param editor registry
    material_setup_param_editor_registry_ =
        std::move(entity_archive_setup_param->material_setup_param_editor_registry);

    // Create entity setup param manager
    entity_setup_param_manager_ = std::make_unique<component_editor::SetupParamManager>(entity_setup_param_map_);

    // Create entity setup param adder
    entity_setup_param_adder_ = std::make_unique<component_editor::SetupParamAdder>(entity_setup_param_map_);

    // Create entity setup param eraser
    entity_setup_param_eraser_ = std::make_unique<component_editor::SetupParamEraser>(entity_setup_param_map_);

    // Create material setup param manager
    material_setup_param_manager_ = std::make_unique<material_editor::SetupParamManager>(material_setup_param_map_);

    // Create material setup param adder
    material_setup_param_adder_ = std::make_unique<material_editor::SetupParamAdder>(material_setup_param_map_);

    // Create material setup param eraser
    material_setup_param_eraser_ = std::make_unique<material_editor::SetupParamEraser>(material_setup_param_map_);

    // Create setup param editor
    entity_setup_param_editor_ = std::make_unique<component_editor::SetupParamEditor>(
        *entity_setup_param_manager_, component_name_map_, component_reflection_registry_,
        std::move(entity_archive_setup_param->setup_param_field_value_setter));

    return true; // Setup successful
}

bool EntityArchiveService::PreUpdate()
{
    // Call base class PreUpdate
    if (!mono_service::Service::PreUpdate())
        return false;

    return true;
}

bool EntityArchiveService::Update()
{
    // Begin frame update
    BeginFrame();

    // Call base class Update
    if (!mono_service::Service::Update())
        return false;

    // Clear edited infos in setup param editor
    entity_setup_param_editor_->ClearEditedInfos();

    while (!GetExecutableCommandQueue().IsEmpty())
    {
        // Dequeue command list
        std::unique_ptr<mono_service::ServiceCommandList> command_list
            = GetExecutableCommandQueue().DequeueCommandList();

        // Execute all commands in the command list
        for (const auto& command : command_list->GetCommands())
        {
            // Execute command
            bool result = command->Execute(GetAPI());
            if (!result)
                return false; // Stop update on failure
        }
    }

    // End frame update
    EndFrame();

    return true; // Update successful
}

bool EntityArchiveService::PostUpdate()
{
    // Call base class PostUpdate
    if (!mono_service::Service::PostUpdate())
        return false;

    return true;
}

std::unique_ptr<mono_service::ServiceCommandList> EntityArchiveService::CreateCommandList()
{
    return std::make_unique<EntityArchiveServiceCommandList>();
}

std::unique_ptr<mono_service::ServiceView> EntityArchiveService::CreateView()
{
    return std::make_unique<EntityArchiveServiceView>(GetAPI());
}

component_editor::SetupParamAdder& EntityArchiveService::GetSetupParamAdder()
{
    assert(IsSetup() && "Service is not set up");
    return *entity_setup_param_adder_;
}

component_editor::SetupParamEraser& EntityArchiveService::GetSetupParamEraser()
{
    assert(IsSetup() && "Service is not set up");
    return *entity_setup_param_eraser_;
}

component_editor::SetupParamEditor& EntityArchiveService::GetSetupParamEditor()
{
    assert(IsSetup() && "Service is not set up");
    return *entity_setup_param_editor_;
}

const material_editor::SetupParamManager& EntityArchiveService::GetMaterialSetupParamManager() const
{
    assert(IsSetup() && "Service is not set up");
    return *material_setup_param_manager_;
}

material_editor::SetupParamAdder& EntityArchiveService::GetMaterialSetupParamAdder()
{
    assert(IsSetup() && "Service is not set up");
    return *material_setup_param_adder_;
}

material_editor::SetupParamEraser& EntityArchiveService::GetMaterialSetupParamEraser()
{
    assert(IsSetup() && "Service is not set up");
    return *material_setup_param_eraser_;
}

bool EntityArchiveService::CheckIsEditable(ecs::Entity entity, ecs::ComponentID component_id) const
{
    // Lock for shared access
    std::shared_lock<std::shared_mutex> lock = LockShared();

    assert(IsSetup() && "Service is not set up");
    return entity_setup_param_editor_->IsEditable(entity, component_id);
}

utility_header::ConstSharedLockedValue<component_editor::FieldMap> EntityArchiveService::GetComponentFieldMap(
    ecs::ComponentID component_id) const
{
    // Lock for shared access
    std::shared_lock<std::shared_mutex> lock = LockShared();

    // Get component name
    auto name_it = component_name_map_.find(component_id);
    assert(name_it != component_name_map_.end() && "Component ID not found in component name map.");
    std::string_view component_name = name_it->second;

    // Find component reflection info
    auto reflection_it = component_reflection_registry_.find(
        component_editor::ComponentReflectionInfo(std::string(component_name), {}));
    assert(reflection_it != component_reflection_registry_.end() && "Component reflection info not found.");
    const component_editor::ComponentReflectionInfo& reflection_info = *reflection_it;

    return utility_header::ConstSharedLockedValue<component_editor::FieldMap>(
        reflection_info.GetFieldMap(), std::move(lock));
}

const uint8_t* EntityArchiveService::GetSetupParamField(
    ecs::Entity entity, ecs::ComponentID component_id, std::string_view field_name) const
{
    // Get component name
    auto name_it = component_name_map_.find(component_id);
    assert(name_it != component_name_map_.end() && "Component ID not found in component name map.");
    std::string_view component_name = name_it->second;

    // Find component reflection info
    auto reflection_it = component_reflection_registry_.find(
        component_editor::ComponentReflectionInfo(std::string(component_name), {}));
    assert(reflection_it != component_reflection_registry_.end() && "Component reflection info not found.");
    const component_editor::ComponentReflectionInfo& reflection_info = *reflection_it;

    // Find field info
    const component_editor::FieldMap& field_map = reflection_info.GetFieldMap();
    auto field_it = field_map.find(std::string(field_name));
    assert(field_it != field_map.end() && "Field name not found in field map.");
    const component_editor::FieldInfo& field_info = field_it->second;

    // Get the setup param
    const ecs::Component::SetupParam* setup_param = entity_setup_param_manager_->GetSetupParam(entity, component_id);
    assert(setup_param != nullptr && "Setup param not found for the entity's component.");

    // Calculate field address
    const uint8_t* base_address = reinterpret_cast<const uint8_t*>(setup_param);
    const uint8_t* field_address = base_address + field_info.offset;

    // Create locked value
    return field_address;
}

utility_header::ConstSharedLockedValue<ecs::Component::SetupParam> EntityArchiveService::GetSetupParam(
    ecs::Entity entity, ecs::ComponentID component_id) const
{
    // Lock for shared access
    std::shared_lock<std::shared_mutex> lock = LockShared();

    // Get the setup param
    const ecs::Component::SetupParam* setup_param = entity_setup_param_manager_->GetSetupParam(entity, component_id);
    assert(setup_param != nullptr && "Setup param not found for the entity's component.");

    // Create locked value
    return utility_header::ConstSharedLockedValue<ecs::Component::SetupParam>(*setup_param, std::move(lock));
}

void EntityArchiveService::ReplaceSetupParam(
    ecs::Entity entity, ecs::ComponentID component_id, std::unique_ptr<ecs::Component::SetupParam> new_setup_param)
{
    // Check if setup param exists
    auto entity_it = entity_setup_param_map_.find(entity);
    assert(entity_it != entity_setup_param_map_.end() && "Entity not found in setup param map.");

    auto component_it = entity_it->second.find(component_id);
    assert(component_it != entity_it->second.end() && "Component ID not found in entity's setup param map.");

    // Replace the setup param
    entity_setup_param_map_[entity][component_id] = std::move(new_setup_param);
}

utility_header::ConstSharedLockedValue<component_editor::ComponentNameMap> EntityArchiveService::GetComponentNameMap() const
{
    // Lock for shared access
    std::shared_lock<std::shared_mutex> lock = LockShared();

    // Create locked value
    return utility_header::ConstSharedLockedValue<component_editor::ComponentNameMap>(
        component_name_map_, std::move(lock));
}

utility_header::ConstSharedLockedValue<component_editor::ComponentAdderMap> EntityArchiveService::GetComponentAdderMap() const
{
    // Lock for shared access
    std::shared_lock<std::shared_mutex> lock = LockShared();

    // Create locked value
    return utility_header::ConstSharedLockedValue<component_editor::ComponentAdderMap>(
        component_adder_map_, std::move(lock));
}

utility_header::ConstSharedLockedValue<ComponentSetupParamFieldTypeRegistry> 
    EntityArchiveService::GetSetupParamFieldTypeRegistry() const
{
    // Lock for shared access
    std::shared_lock<std::shared_mutex> lock = LockShared();

    // Create locked value
    return utility_header::ConstSharedLockedValue<ComponentSetupParamFieldTypeRegistry>(
        setup_param_field_type_registry_, std::move(lock));
}

bool EntityArchiveService::CheckComponentEditable(ecs::ComponentID component_id) const
{
    // Lock for shared access
    std::shared_lock<std::shared_mutex> lock = LockShared();

    // Find component name
    auto name_it = component_name_map_.find(component_id);
    if (name_it == component_name_map_.end())
        return false; // Not editable if component name not found

    // Find component reflection info
    auto reflection_it = component_reflection_registry_.find(
        component_editor::ComponentReflectionInfo(std::string(name_it->second), {}));
    if (reflection_it == component_reflection_registry_.end())
        return false; // Not editable if reflection info not found

    return true; // Editable
}

std::vector<component_editor::EditedInfo> EntityArchiveService::GetEditedInfos() const
{
    // Lock for shared access
    std::shared_lock<std::shared_mutex> lock = LockShared();

    assert(IsSetup() && "Service is not set up");
    return entity_setup_param_editor_->GetEditedInfos();
}

utility_header::ConstSharedLockedValue<MaterialSetupParamEditorRegistry> 
    EntityArchiveService::GetMaterialSetupParamEditorRegistry() const
{
    // Lock for shared access
    std::shared_lock<std::shared_mutex> lock = LockShared();

    // Create locked value
    return utility_header::ConstSharedLockedValue<MaterialSetupParamEditorRegistry>(
        material_setup_param_editor_registry_, std::move(lock));
}

void EntityArchiveService::ReplaceSetupParam(
    render_graph::MaterialHandle* material_handle, 
    std::unique_ptr<material_editor::SetupParamWrapper> new_setup_param)
{
    // Check if setup param exists
    auto material_it = material_setup_param_map_.find(*material_handle);
    assert(material_it != material_setup_param_map_.end() && "Material handle not found in setup param map.");

    // Replace the setup param
    material_setup_param_map_[*material_handle] = std::move(new_setup_param);
}

MONO_ENTITY_ARCHIVE_SERVICE_DLL void AddSetupParam(
    mono_service::ServiceProxyManager &service_proxy_manager, 
    ecs::Entity entity, ecs::ComponentID component_id, std::unique_ptr<ecs::Component::SetupParam> setup_param)
{
    // Get entity archive service proxy
    std::unique_ptr<mono_service::ServiceProxy> entity_archive_service_proxy;
    service_proxy_manager.WithLock([&](mono_service::ServiceProxyManager& manager)
    {
        entity_archive_service_proxy = manager.GetServiceProxy(EntityArchiveServiceHandle::ID()).Clone();
    });

    // Create entity archive service command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list = entity_archive_service_proxy->CreateCommandList();
    EntityArchiveServiceCommandList* entity_archive_command_list
        = dynamic_cast<EntityArchiveServiceCommandList*>(command_list.get());
    assert(entity_archive_command_list != nullptr && "Entity archive service command list is null!");

    // Create entity archive service view
    std::unique_ptr<mono_service::ServiceView> service_view = entity_archive_service_proxy->CreateView();
    EntityArchiveServiceView* entity_archive_service_view
        = dynamic_cast<EntityArchiveServiceView*>(service_view.get());
    assert(entity_archive_service_view != nullptr && "Entity archive service view is null!");

    // Check if TransformComponent is editable
    bool is_editable = entity_archive_service_view->CheckComponentEditable(component_id);

    // If editable, add command to set initial setup parameters
    if (is_editable)
    {
        entity_archive_command_list->AddSetupParam(entity, component_id, std::move(setup_param));

        // Submit the command list
        entity_archive_service_proxy->SubmitCommandList(std::move(command_list));
    }
}

MONO_ENTITY_ARCHIVE_SERVICE_DLL void AddSetupParam(
    mono_service::ServiceProxy &entity_archive_service_proxy, 
    ecs::Entity entity, ecs::ComponentID component_id, std::unique_ptr<ecs::Component::SetupParam> setup_param)
{
    // Create entity archive service command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list = entity_archive_service_proxy.CreateCommandList();
    EntityArchiveServiceCommandList* entity_archive_command_list
        = dynamic_cast<EntityArchiveServiceCommandList*>(command_list.get());
    assert(entity_archive_command_list != nullptr && "Entity archive service command list is null!");

    // Create entity archive service view
    std::unique_ptr<mono_service::ServiceView> service_view = entity_archive_service_proxy.CreateView();
    EntityArchiveServiceView* entity_archive_service_view
        = dynamic_cast<EntityArchiveServiceView*>(service_view.get());
    assert(entity_archive_service_view != nullptr && "Entity archive service view is null!");

    // Check if TransformComponent is editable
    bool is_editable = entity_archive_service_view->CheckComponentEditable(component_id);

    // If editable, add command to set initial setup parameters
    if (is_editable)
    {
        entity_archive_command_list->AddSetupParam(entity, component_id, std::move(setup_param));

        // Submit the command list
        entity_archive_service_proxy.SubmitCommandList(std::move(command_list));
    }
}

} // namespace mono_entity_archive_service