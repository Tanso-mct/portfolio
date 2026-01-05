#include "mono_entity_archive_extension/src/pch.h"
#include "mono_entity_archive_extension/include/import_helper.h"

#include "utility_header/logger.h"

#include "mono_entity_archive_service/include/entity_archive_service_view.h"
#include "mono_entity_archive_service/include/export_config.h"

namespace mono_entity_archive_extension
{

MONO_ENTITY_ARCHIVE_EXT_DLL bool CreateEntitiesFromExportedJSON(
    ecs::World& world, const nlohmann::json& json, mono_service::ServiceProxyManager& service_proxy_manager,
    std::vector<ecs::Entity>* out_created_entities)
{
    // Get entity archive service proxy
    std::unique_ptr<mono_service::ServiceProxy> entity_archive_service_proxy = nullptr;
    service_proxy_manager.WithLock([&](mono_service::ServiceProxyManager& manager)
    {
        entity_archive_service_proxy = manager.GetServiceProxy(
            mono_entity_archive_service::EntityArchiveServiceHandle::ID()).Clone();
    });

    // Create entity archive service view
    std::unique_ptr<mono_service::ServiceView> service_view 
        = entity_archive_service_proxy->CreateView();
    mono_entity_archive_service::EntityArchiveServiceView* entity_archive_service_view 
        = dynamic_cast<mono_entity_archive_service::EntityArchiveServiceView*>(service_view.get());
    assert(entity_archive_service_view != nullptr && "Entity archive service view is null!");

    // Get component name map
    const component_editor::ComponentNameMap& component_name_map
        = entity_archive_service_view->GetComponentNameMap();

    // Get component adder map
    const component_editor::ComponentAdderMap& component_adder_map
        = entity_archive_service_view->GetComponentAdderMap();

    // Create component name to id map
    std::unordered_map<std::string, ecs::ComponentID> component_name_to_id_map;
    for (const auto& [component_id, component_name] : component_name_map)
        component_name_to_id_map[component_name] = component_id;

    // Get entities JSON
    const nlohmann::json& entities_json = json[mono_entity_archive_service::EXPORT_TAG_ENTITIES];

    // Prepare output created entities vector
    std::vector<ecs::Entity> created_entities;

    for (const auto& entity_json : entities_json)
    {
        // Create new entity
        ecs::Entity entity = world.CreateEntity();
        created_entities.push_back(entity);

        for (auto it = entity_json.begin(); it != entity_json.end(); ++it)
        {
            const std::string& component_name = it.key();
            const nlohmann::json& component_json = it.value();

            // Get component ID
            auto component_id_it = component_name_to_id_map.find(component_name);
            if (component_id_it == component_name_to_id_map.end())
            {
                utility_header::ConsoleLogErr(
                    {"Component name not found: ", component_name}, __FILE__, __LINE__, __FUNCTION__);
                return false; // Component name not found
            }
            ecs::ComponentID component_id = component_id_it->second;

            // Get component field map
            const component_editor::FieldMap& component_field_map
                = entity_archive_service_view->GetComponentFieldMap(component_id);

            // Get component adder
            if (component_adder_map.find(component_id) == component_adder_map.end())
            {
                utility_header::ConsoleLogErr(
                    {"Component adder not found for component ID: ", std::to_string(component_id)},
                    __FILE__, __LINE__, __FUNCTION__);
                return false; // Component adder not found
            }
            const component_editor::ComponentAdder& component_adder = *(component_adder_map.at(component_id));

            // Create setup param using component adder
            std::unique_ptr<ecs::Component::SetupParam> setup_param 
                = component_adder.GetSetupParam(service_proxy_manager);

            for (auto field_it = component_json.begin(); field_it != component_json.end(); ++field_it)
            {
                const std::string& field_name = field_it.key();
                const nlohmann::json& field_value_json = field_it.value();

                // Get type name of the field
                if (component_field_map.find(field_name) == component_field_map.end())
                {
                    utility_header::ConsoleLogErr(
                        {"Field name not found in component field map: ", field_name}, 
                        __FILE__, __LINE__, __FUNCTION__);
                    return false; // Field name not found
                }
                const std::string& field_type_name = component_field_map.at(field_name).type_name;

                // Get field value import function
                const mono_entity_archive_service::ComponentSetupParamAnyFieldImportFunc& import_func
                    = entity_archive_service_view->GetSetupParamFieldTypeRegistry().GetSetupParamFieldImportFunc(
                        field_type_name);

                // Import field value from JSON
                std::any field_value_any = import_func(field_name, field_value_json, service_proxy_manager);

                // Set field value in setup param
                bool set_result = entity_archive_service_view->GetSetupParamEditor().SetFieldValue(
                    setup_param.get(), field_type_name, 
                    component_field_map.at(field_name).offset, field_value_any);
                if (!set_result)
                {
                    utility_header::ConsoleLogErr(
                        {"Failed to set field value for field: ", field_name}, 
                        __FILE__, __LINE__, __FUNCTION__);
                    return false; // Failed to set field value
                }
            }

            // Add component to the entity by using the component adder
            bool add_result = component_adder.Add(world, entity, std::move(setup_param), service_proxy_manager);
            if (!add_result)
            {
                utility_header::ConsoleLogErr(
                    {"Failed to add component: ", component_name, " to entity."}, 
                    __FILE__, __LINE__, __FUNCTION__);
                return false; // Failed to add component
            }
        }
    }

    // Output created entities if requested
    if (out_created_entities != nullptr)
        *out_created_entities = std::move(created_entities);

    return true; // Successfully created entities from JSON
}

} // namespace mono_entity_archive_extension