#pragma once

#include "material_editor/include/dll_config.h"
#include "material_editor/include/setup_param_editor.h"

#include "render_graph/include/material_handle.h"

namespace material_editor
{

// The registry for setup param editors
template <typename... Args>
class SetupParamEditorRegistry
{
public:
    SetupParamEditorRegistry() = default;
    ~SetupParamEditorRegistry() = default;

    // Register a setup param editor function for a material type handle ID
    void RegisterSetupParamEditor(
        render_graph::MaterialTypeHandleID material_type_handle_id, 
        std::string material_type_name,
        SetupParamEditFunc<Args...> edit_func, MaterialCreateFunc<Args...> create_func,
        SetupParamExportFunc<Args...> export_func, MaterialImportFunc<Args...> import_func)
    {
        // Register material type name
        assert(
            material_type_name_map_.find(material_type_handle_id) == material_type_name_map_.end()
            && "Material type name already registered for this material type handle ID.");
        material_type_name_map_[material_type_handle_id] = std::move(material_type_name);

        // Register edit function
        assert(
            edit_func_map_.find(material_type_handle_id) == edit_func_map_.end()
            && "Setup param editor already registered for this material type handle ID.");
        edit_func_map_[material_type_handle_id] = std::move(edit_func);

        // Register create function
        assert(
            create_func_map_.find(material_type_handle_id) == create_func_map_.end()
            && "Setup param creator already registered for this material type handle ID.");
        create_func_map_[material_type_handle_id] = std::move(create_func);

        // Register export function
        assert(
            export_func_map_.find(material_type_handle_id) == export_func_map_.end()
            && "Setup param exporter already registered for this material type handle ID.");
        export_func_map_[material_type_handle_id] = std::move(export_func);

        // Register import function
        assert(
            import_func_map_.find(material_type_handle_id) == import_func_map_.end()
            && "Setup param importer already registered for this material type handle ID.");
        import_func_map_[material_type_handle_id] = std::move(import_func);
    }

    // Get the material type name for a material type handle ID
    const std::string& GetMaterialTypeName(render_graph::MaterialTypeHandleID material_type_handle_id) const
    {
        auto it = material_type_name_map_.find(material_type_handle_id);
        assert(it != material_type_name_map_.end() && "Material type name not found for this material type handle ID.");

        return it->second;
    }
        
    // Get the setup param editor function for a material type handle ID
    const SetupParamEditFunc<Args...>& GetSetupParamEditor(render_graph::MaterialTypeHandleID material_type_handle_id) const
    {
        auto it = edit_func_map_.find(material_type_handle_id);
        assert(it != edit_func_map_.end() && "Setup param editor not found for this material type handle ID.");

        return it->second;
    }

    // Get the setup param creator function for a material type handle ID
    const MaterialCreateFunc<Args...>& GetSetupParamCreator(render_graph::MaterialTypeHandleID material_type_handle_id) const
    {
        auto it = create_func_map_.find(material_type_handle_id);
        assert(it != create_func_map_.end() && "Setup param creator not found for this material type handle ID.");

        return it->second;
    }

    // Get the setup param exporter function for a material type handle ID
    const SetupParamExportFunc<Args...>& GetSetupParamExporter(render_graph::MaterialTypeHandleID material_type_handle_id) const
    {
        auto it = export_func_map_.find(material_type_handle_id);
        assert(it != export_func_map_.end() && "Setup param exporter not found for this material type handle ID.");

        return it->second;
    }

    // Get the setup param importer function for a material type handle ID
    const MaterialImportFunc<Args...>& GetSetupParamImporter(render_graph::MaterialTypeHandleID material_type_handle_id) const
    {
        auto it = import_func_map_.find(material_type_handle_id);
        assert(it != import_func_map_.end() && "Setup param importer not found for this material type handle ID.");

        return it->second;
    }

    // Get all registered material type handle IDs
    std::vector<render_graph::MaterialTypeHandleID> GetRegisteredMaterialTypeHandleIDs() const
    {
        std::vector<render_graph::MaterialTypeHandleID> material_type_handle_ids;
        for (const auto& pair : material_type_name_map_)
            material_type_handle_ids.push_back(pair.first);
            
        return material_type_handle_ids;
    }

private:
    // The map from material type handle ID to material type name
    std::unordered_map<render_graph::MaterialTypeHandleID, std::string> material_type_name_map_;

    // The map from material type handle ID to setup param edit function
    std::unordered_map<render_graph::MaterialTypeHandleID, SetupParamEditFunc<Args...>> edit_func_map_;

    // The map from material type handle ID to setup param create function
    std::unordered_map<render_graph::MaterialTypeHandleID, MaterialCreateFunc<Args...>> create_func_map_;

    // The map from material type handle ID to setup param export function
    std::unordered_map<render_graph::MaterialTypeHandleID, SetupParamExportFunc<Args...>> export_func_map_;

    // The map from material type handle ID to setup param import function
    std::unordered_map<render_graph::MaterialTypeHandleID, MaterialImportFunc<Args...>> import_func_map_;
};

// The registrar for setup param editors
template <typename... Args>
class SetupParamEditorRegistrar
{
public:
    // Register to the registry upon construction
    SetupParamEditorRegistrar(
        SetupParamEditorRegistry<Args...>& registry,
        render_graph::MaterialTypeHandleID material_type_handle_id, 
        std::string material_type_name,
        SetupParamEditFunc<Args...> edit_func, MaterialCreateFunc<Args...> create_func,
        SetupParamExportFunc<Args...> export_func, MaterialImportFunc<Args...> import_func)
    {
        registry.RegisterSetupParamEditor(
            material_type_handle_id, material_type_name, 
            std::move(edit_func), std::move(create_func), std::move(export_func), std::move(import_func));
    }
};

} // namespace material_editor