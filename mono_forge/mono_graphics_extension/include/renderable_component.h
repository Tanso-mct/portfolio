#pragma once

#include "ecs/include/component.h"
#include "render_graph/include/resource_handle.h"
#include "render_graph/include/material_handle.h"
#include "mono_service/include/service.h"
#include "mono_graphics_extension/include/dll_config.h"
#include "asset_loader/include/asset_handle.h"

namespace mono_graphics_extension
{

// The handle class for the renderable component
class MONO_GRAPHICS_EXT_DLL RenderableComponentHandle :
    public ecs::ComponentHandle<RenderableComponentHandle> {};

// A struct representing a set of mesh data
struct MeshSet
{
    asset_loader::AssetHandleID mesh_asset_handle = asset_loader::AssetHandleID();
    std::vector<render_graph::ResourceHandle> vertex_buffer_handles;
    std::vector<render_graph::ResourceHandle> index_buffer_handles;
    std::vector<uint32_t> index_counts;
    std::vector<const render_graph::MaterialHandle*> material_handles;
};

// The renderable component class
class MONO_GRAPHICS_EXT_DLL RenderableComponent :
    public ecs::Component
{
public:
    RenderableComponent(std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy);
    virtual ~RenderableComponent() override;

    class SetupParam : //REFLECTABLE_CLASS_BEGIN//
        public ecs::Component::SetupParam
    {
    public:
        SetupParam() :
            cast_shadow_(true) // Default to cast shadows
        {
        }

        virtual ~SetupParam() override = default;

        // Mesh set
        MeshSet mesh_set_; //REFLECTED//

        // Whether the renderable casts shadows
        bool cast_shadow_; //REFLECTED//

    }; //REFLECTABLE_CLASS_END//
    
    virtual bool Setup(ecs::Component::SetupParam& param) override;
    virtual bool Apply(const ecs::Component::SetupParam& param) override;
    virtual ecs::ComponentID GetID() const override;

    // Get vertex buffer handle
    const std::vector<render_graph::ResourceHandle>* GetVertexBufferHandles() const { return &vertex_buffer_handles_; }

    // Set vertex buffer handle
    void SetVertexBufferHandles(std::vector<render_graph::ResourceHandle> handles) { vertex_buffer_handles_ = std::move(handles); }

    // Get index buffer handle
    const std::vector<render_graph::ResourceHandle>* GetIndexBufferHandles() const { return &index_buffer_handles_; }

    // Set index buffer handle
    void SetIndexBufferHandles(std::vector<render_graph::ResourceHandle> handles) { index_buffer_handles_ = std::move(handles); }

    // Get index count
    const std::vector<uint32_t>* GetIndexCounts() const { return &index_counts_; }

    // Set index count
    void SetIndexCounts(std::vector<uint32_t> counts) { index_counts_ = std::move(counts); }

    // Get material handle
    std::vector<const render_graph::MaterialHandle*> GetMaterialHandles() const { return material_handles_; }

    // Set material handle
    void SetMaterialHandles(std::vector<const render_graph::MaterialHandle*> handle) { material_handles_ = handle; }

    // Get world matrix buffer handle
    const render_graph::ResourceHandle* GetWorldMatrixBufferHandle() const { return &world_matrix_buffer_handle_; }

    // Get whether the renderable casts shadows
    bool NeedsCastShadow() const { return cast_shadow_; }

    // Set whether the renderable casts shadows
    void SetNeedsCastShadow(bool cast_shadow) { cast_shadow_ = cast_shadow; }

private:
    // The graphics service proxy
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy_ = nullptr;

    // The vertex buffer handles
    std::vector<render_graph::ResourceHandle> vertex_buffer_handles_ = {};

    // The index buffer handles
    std::vector<render_graph::ResourceHandle> index_buffer_handles_ = {};

    // The number of indices
    std::vector<uint32_t> index_counts_ = {};

    // The material handles
    std::vector<const render_graph::MaterialHandle*> material_handles_;

    // The world matrix buffer handle
    render_graph::ResourceHandle world_matrix_buffer_handle_ = render_graph::ResourceHandle();

    // Whether the renderable casts shadows
    bool cast_shadow_ = true;
};

} // namespace mono_graphics_extension