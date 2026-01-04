#pragma once

#include <unordered_map>

#include "mono_render/include/dll_config.h"
#include "class_template/singleton.h"
#include "render_graph/include/material_handle.h"

namespace mono_render
{

// The manager class for material handles
class MONO_RENDER_API MaterialHandleManager :
    public class_template::Singleton<MaterialHandleManager>
{
public:
    MaterialHandleManager() = default;
    virtual ~MaterialHandleManager() override = default;

    // Register a material handle with a given ID
    void RegisterMaterialHandle(
        uint32_t material_handle_id, 
        render_graph::MaterialHandle material_handle = render_graph::MaterialHandle());

    // Get a material handle by its ID
    render_graph::MaterialHandle* GetMaterialHandle(uint32_t material_handle_id);

private:
    // The map of material handles
    std::unordered_map<uint32_t, render_graph::MaterialHandle> material_handles_;
};

// The ID generator for material handles
class MONO_RENDER_API MaterialHandleIDGenerator :
    public class_template::Singleton<MaterialHandleIDGenerator>
{
public:
    MaterialHandleIDGenerator() = default;
    virtual ~MaterialHandleIDGenerator() override = default;

    // Generate a new unique material handle ID
    uint32_t Generate() { return next_id_++; }

private:
    uint32_t next_id_ = 1; // Start from 1 to avoid using 0 as a valid ID
};

// The template class for material handle ids of specific types
template <typename Tag>
class MaterialHandleID
{
public:
    MaterialHandleID() = default;
    virtual ~MaterialHandleID() = default;

    // Get the unique ID of the asset handle type
    static uint32_t ID()
    {
        static const uint32_t id = MaterialHandleIDGenerator::GetInstance().Generate();
        return id;
    }
};

} // namespace mono_render