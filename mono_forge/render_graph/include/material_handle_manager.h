#pragma once

#include <unordered_map>
#include <string>
#include <string_view>

#include "render_graph/include/dll_config.h"
#include "class_template/singleton.h"
#include "render_graph/include/material_handle.h"

namespace render_graph
{

// The default material name
constexpr const char* DEFAULT_MATERIAL_NAME = "Unnamed";

// The key type for material handles
using MaterialHandleKey = uint32_t;

// The manager class for material handles
class RENDER_GRAPH_DLL MaterialHandleManager :
    public class_template::Singleton<MaterialHandleManager>
{
public:
    MaterialHandleManager() = default;
    virtual ~MaterialHandleManager() override = default;

    // Register a material handle with a given ID
    void RegisterMaterialHandle(
        MaterialHandleKey material_handle_key, 
        render_graph::MaterialHandle material_handle = render_graph::MaterialHandle(),
        std::string_view material_name = DEFAULT_MATERIAL_NAME);

    // Unregister a material handle by its ID
    void UnregisterMaterialHandle(MaterialHandleKey material_handle_key);

    // Get a material handle by its ID
    render_graph::MaterialHandle* GetMaterialHandle(MaterialHandleKey material_handle_key);

    // Get the name of a material by its ID
    std::string_view GetMaterialHandleName(MaterialHandleKey material_handle_key) const;

    // Get all registered material handle IDs
    std::vector<MaterialHandleKey> GetRegisteredMaterialHandleKeys() const;

    // Set the name of a material by its ID
    void SetMaterialHandleName(MaterialHandleKey material_handle_key, std::string_view material_name);

private:
    // The map of material handles
    std::unordered_map<MaterialHandleKey, render_graph::MaterialHandle> material_handles_;

    // The map of material names
    std::unordered_map<MaterialHandleKey, std::string> material_names_;
};

// The ID generator for material handles
class RENDER_GRAPH_DLL MaterialHandleKeyGenerator :
    public class_template::Singleton<MaterialHandleKeyGenerator>
{
public:
    MaterialHandleKeyGenerator() = default;
    virtual ~MaterialHandleKeyGenerator() override = default;

    // Generate a new unique material handle key
    MaterialHandleKey Generate() { return next_key_++; }

private:
    MaterialHandleKey next_key_ = 1; // Start from 1 to avoid using 0 as a valid key
};

// The template class for material handle keys
template <typename Tag>
class MaterialHandleKeyFor
{
public:
    MaterialHandleKeyFor() = default;
    virtual ~MaterialHandleKeyFor() = default;

    // Get the unique ID of the asset handle type
    static MaterialHandleKey Value()
    {
        static const MaterialHandleKey id = MaterialHandleKeyGenerator::GetInstance().Generate();
        return id;
    }
};

} // namespace render_graph