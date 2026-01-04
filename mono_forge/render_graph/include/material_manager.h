#pragma once

#include <unordered_set>
#include <memory>

#include "class_template/singleton.h"
#include "utility_header/container.h"

#include "render_graph/include/dll_config.h"
#include "render_graph/include/material.h"
#include "render_graph/include/material_handle.h"

namespace render_graph
{

// The container type used to contain materials
using MaterialContainer = utility_header::Container<Material>;

// The resource manager class that manages materials and their handles
class RENDER_GRAPH_DLL MaterialManager :
    public class_template::Singleton<MaterialManager>
{
public:
    MaterialManager(MaterialContainer& container) : container_(container) {}
    ~MaterialManager() override = default;

    // Execute a function with a unique lock on the resource container
    template <typename Func>
    void WithLock(Func&& func) 
    {
        container_.WithUniqueLock([&](MaterialContainer& container) 
        {
            func(*this);
        });
    }

    // Get a material using its handle
    Material& GetMaterial(const MaterialHandle* handle);

    // Get a const material using its handle
    const Material& GetMaterial(const MaterialHandle* handle) const;

    // Wrap the Contains method of the container
    // It must be called in WithLock lambda function
    bool Contains(const MaterialHandle* handle) const;

private:
    // The resource container reference
    MaterialContainer& container_;
};

class RENDER_GRAPH_DLL MaterialAdder
{
public:
    MaterialAdder(MaterialContainer& container) : container_(container) {}
    ~MaterialAdder() = default;

    // Adds a new resource and returns its associated MaterialHandle
    MaterialHandle AddMaterial(std::unique_ptr<Material> resource);

private:
    // The resource container reference
    MaterialContainer& container_;
};

class RENDER_GRAPH_DLL MaterialEraser
{
public:
    MaterialEraser(MaterialContainer& container) : container_(container) {}
    ~MaterialEraser() = default;

    // Erase a resource associated with the given MaterialHandle
    void EraseMaterial(const MaterialHandle* handle);

private:
    // The resource container reference
    MaterialContainer& container_;
};

} // namespace render_graph