#include "mono_entity_archive_service/src/pch.h"
#include "mono_entity_archive_service/include/entity_archive_service_command_list.h"

#include "utility_header/logger.h"

#include "mono_entity_archive_service/include/export_config.h"

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

void EntityArchiveServiceCommandList::ExportComponentSetupParamsToFile(
    const std::string& file_path, std::vector<ecs::Entity> entities,
    mono_service::ServiceProxyManager& service_proxy_manager)
{
    AddCommand([file_path, entities = std::move(entities), &service_proxy_manager](mono_service::ServiceAPI& api)
    {
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, EntityArchiveServiceAPI>::value,
            "EntityArchiveServiceAPI must be derived from ServiceAPI.");
        EntityArchiveServiceAPI& entity_archive_service_api = dynamic_cast<EntityArchiveServiceAPI&>(api);

        // Prepare JSON data for export
        nlohmann::json json_data;
        json_data[EXPORT_TAG_ENTITIES] = nlohmann::json::array();

        for (const ecs::Entity& entity : entities)
        {
            // Create JSON object for the entity
            nlohmann::json entity_json;

            // Get all component IDs which entity has
            std::vector<ecs::ComponentID> component_ids 
                = entity_archive_service_api.GetSetupParamManager().GetComponentIDs(entity);

            for (ecs::ComponentID component_id : component_ids)
            {
                // Create JSON object for the component
                nlohmann::json component_json;

                // Get component name
                std::string_view component_name = entity_archive_service_api.GetComponentNameMap().at(component_id);                

                // Get component field map
                const component_editor::FieldMap& field_map 
                    = entity_archive_service_api.GetComponentFieldMap(component_id);

                // Get setup param
                const ecs::Component::SetupParam& setup_param
                    = entity_archive_service_api.GetSetupParam(entity, component_id);

                for (const auto& [field_name, field_info] : field_map)
                {
                    // Get setup param field value
                    const uint8_t* field_value 
                        = entity_archive_service_api.GetSetupParamField(entity, component_id, field_name);

                    // Get field create function
                    auto field_type_create_func 
                        = entity_archive_service_api.GetSetupParamFieldTypeRegistry().GetSetupParamFieldCreateFunc(
                            field_info.type_name);
                    assert(field_type_create_func && "Field type create function not found");

                    // Create std::any value from field value
                    std::any value = field_type_create_func(field_value, service_proxy_manager);

                    // Get field export function
                    auto field_type_export_func 
                        = entity_archive_service_api.GetSetupParamFieldTypeRegistry().GetSetupParamFieldExportFunc(
                            field_info.type_name);

                    // Export field value to JSON
                    nlohmann::json field_json = field_type_export_func(value, field_name, service_proxy_manager);

                    // Add field JSON to component JSON
                    component_json[field_name.data()] = field_json;
                }

                // Add component name to component JSON
                entity_json[component_name.data()] = component_json;
            }

            // Add entity JSON to JSON data
            json_data[EXPORT_TAG_ENTITIES].push_back(entity_json);
        }

        // Write JSON data to file
        std::ofstream output_file(file_path);
        if (!output_file.is_open())
        {
            utility_header::ConsoleLogErr(
                {"Failed to open file for writing: ", file_path}, __FILE__, __LINE__, __FUNCTION__);
            return false; // Failure
        }

        output_file << json_data.dump(4);
        if (!output_file)
        {
            utility_header::ConsoleLogErr(
                {"Failed to write JSON data to file: ", file_path}, __FILE__, __LINE__, __FUNCTION__);
            return false; // Failure
        }

        // Close the file
        output_file.close();

        return true; // Success
    });
}

void EntityArchiveServiceCommandList::ExportComponentSetupParamsToFile(
    const std::wstring& file_path, std::vector<ecs::Entity> entities,
    mono_service::ServiceProxyManager& service_proxy_manager)
{
    AddCommand([file_path, entities = std::move(entities), &service_proxy_manager](mono_service::ServiceAPI& api)
    {
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, EntityArchiveServiceAPI>::value,
            "EntityArchiveServiceAPI must be derived from ServiceAPI.");
        EntityArchiveServiceAPI& entity_archive_service_api = dynamic_cast<EntityArchiveServiceAPI&>(api);

        // Prepare JSON data for export
        nlohmann::json json_data;
        json_data[EXPORT_TAG_ENTITIES] = nlohmann::json::array();

        for (const ecs::Entity& entity : entities)
        {
            // Create JSON object for the entity
            nlohmann::json entity_json;

            // Get all component IDs which entity has
            std::vector<ecs::ComponentID> component_ids 
                = entity_archive_service_api.GetSetupParamManager().GetComponentIDs(entity);

            for (ecs::ComponentID component_id : component_ids)
            {
                // Create JSON object for the component
                nlohmann::json component_json;

                // Get component name
                std::string_view component_name = entity_archive_service_api.GetComponentNameMap().at(component_id);                

                // Get component field map
                const component_editor::FieldMap& field_map 
                    = entity_archive_service_api.GetComponentFieldMap(component_id);

                // Get setup param
                const ecs::Component::SetupParam& setup_param
                    = entity_archive_service_api.GetSetupParam(entity, component_id);

                for (const auto& [field_name, field_info] : field_map)
                {
                    // Get setup param field value
                    const uint8_t* field_value 
                        = entity_archive_service_api.GetSetupParamField(entity, component_id, field_name);

                    // Get field create function
                    auto field_type_create_func 
                        = entity_archive_service_api.GetSetupParamFieldTypeRegistry().GetSetupParamFieldCreateFunc(
                            field_info.type_name);
                    assert(field_type_create_func && "Field type create function not found");

                    // Create std::any value from field value
                    std::any value = field_type_create_func(field_value, service_proxy_manager);

                    // Get field export function
                    auto field_type_export_func 
                        = entity_archive_service_api.GetSetupParamFieldTypeRegistry().GetSetupParamFieldExportFunc(
                            field_info.type_name);

                    // Export field value to JSON
                    nlohmann::json field_json = field_type_export_func(value, field_name, service_proxy_manager);

                    // Add field JSON to component JSON
                    component_json[field_name.data()] = field_json;
                }

                // Add component name to component JSON
                entity_json[component_name.data()] = component_json;
            }

            // Add entity JSON to JSON data
            json_data[EXPORT_TAG_ENTITIES].push_back(entity_json);
        }

        // Write JSON data to file
        std::ofstream output_file(file_path);
        if (!output_file.is_open())
        {
            utility_header::ConsoleLogErr(
                {"Failed to open file for writing"}, __FILE__, __LINE__, __FUNCTION__);
            return false; // Failure
        }

        output_file << json_data.dump(4);
        if (!output_file)
        {
            utility_header::ConsoleLogErr(
                {"Failed to write JSON data to file"}, __FILE__, __LINE__, __FUNCTION__);
            return false; // Failure
        }

        // Close the file
        output_file.close();

        return true; // Success
    });
}

void EntityArchiveServiceCommandList::ExportMaterialSetupParamsToFile(
    const std::string& file_path, std::vector<render_graph::MaterialHandle> material_handles,
    mono_service::ServiceProxyManager& service_proxy_manager)
{
    AddCommand([file_path, material_handles = std::move(material_handles), &service_proxy_manager](mono_service::ServiceAPI& api) mutable
    {
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, EntityArchiveServiceAPI>::value,
            "EntityArchiveServiceAPI must be derived from ServiceAPI.");
        EntityArchiveServiceAPI& entity_archive_service_api = dynamic_cast<EntityArchiveServiceAPI&>(api);

        // Prepare JSON data for export
        nlohmann::json json_data;
        json_data[EXPORT_TAG_MATERIALS] = nlohmann::json::array();

        for (render_graph::MaterialHandle& material_handle : material_handles)
        {
            // Get setup param
            const material_editor::SetupParamWrapper* setup_param
                = entity_archive_service_api.GetMaterialSetupParamManager().GetSetupParam(&material_handle);

            // Get material setup param editor registry
            const MaterialSetupParamEditorRegistry& material_setup_param_editor_registry
                = entity_archive_service_api.GetMaterialSetupParamEditorRegistry();

            // Get export function for the material type
            const MaterialSetupParamExportFunc& export_func
                = material_setup_param_editor_registry.GetSetupParamExporter(
                    setup_param->GetSetupParam()->GetMaterialTypeHandleID());

            // Export setup param to JSON
            nlohmann::json setup_param_json = export_func(setup_param, service_proxy_manager);

            // Add material JSON to JSON data
            json_data[EXPORT_TAG_MATERIALS].push_back(setup_param_json);
        }

        // Write JSON data to file
        std::ofstream output_file(file_path);
        if (!output_file.is_open())
        {
            utility_header::ConsoleLogErr(
                {"Failed to open file for writing"}, __FILE__, __LINE__, __FUNCTION__);
            return false; // Failure
        }

        output_file << json_data.dump(4);
        if (!output_file)
        {
            utility_header::ConsoleLogErr(
                {"Failed to write JSON data to file"}, __FILE__, __LINE__, __FUNCTION__);
            return false; // Failure
        }

        // Close the file
        output_file.close();

        return true; // Success
    });
}

void EntityArchiveServiceCommandList::ExportMaterialSetupParamsToFile(
    const std::wstring& file_path, std::vector<render_graph::MaterialHandle> material_handles,
    mono_service::ServiceProxyManager& service_proxy_manager)
{
    AddCommand([file_path, material_handles = std::move(material_handles), &service_proxy_manager](mono_service::ServiceAPI& api) mutable
    {
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, EntityArchiveServiceAPI>::value,
            "EntityArchiveServiceAPI must be derived from ServiceAPI.");
        EntityArchiveServiceAPI& entity_archive_service_api = dynamic_cast<EntityArchiveServiceAPI&>(api);

        // Prepare JSON data for export
        nlohmann::json json_data;
        json_data[EXPORT_TAG_MATERIALS] = nlohmann::json::array();

        for (render_graph::MaterialHandle& material_handle : material_handles)
        {
            // Get setup param
            const material_editor::SetupParamWrapper* setup_param
                = entity_archive_service_api.GetMaterialSetupParamManager().GetSetupParam(&material_handle);

            // Get material setup param editor registry
            const MaterialSetupParamEditorRegistry& material_setup_param_editor_registry
                = entity_archive_service_api.GetMaterialSetupParamEditorRegistry();

            // Get export function for the material type
            const MaterialSetupParamExportFunc& export_func
                = material_setup_param_editor_registry.GetSetupParamExporter(
                    setup_param->GetSetupParam()->GetMaterialTypeHandleID());

            // Export setup param to JSON
            nlohmann::json setup_param_json = export_func(setup_param, service_proxy_manager);

            // Add material JSON to JSON data
            json_data[EXPORT_TAG_MATERIALS].push_back(setup_param_json);
        }

        // Write JSON data to file
        std::ofstream output_file(file_path);
        if (!output_file.is_open())
        {
            utility_header::ConsoleLogErr(
                {"Failed to open file for writing"}, __FILE__, __LINE__, __FUNCTION__);
            return false; // Failure
        }

        output_file << json_data.dump(4);
        if (!output_file)
        {
            utility_header::ConsoleLogErr(
                {"Failed to write JSON data to file"}, __FILE__, __LINE__, __FUNCTION__);
            return false; // Failure
        }

        // Close the file
        output_file.close();

        return true; // Success
    });
}

} // namespace mono_entity_archive_service