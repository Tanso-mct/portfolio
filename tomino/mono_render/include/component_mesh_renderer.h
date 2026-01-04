#pragma once
#include "mono_render/include/dll_config.h"
#include "riaecs/riaecs.h"

#include "render_graph/include/resource_handle.h"
#include "render_graph/include/material_handle.h"
#include "mono_service/include/service.h"

namespace mono_render
{
    constexpr size_t ComponentMeshRendererMaxCount = 5000;
    class MONO_RENDER_API ComponentMeshRenderer
    {
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

    public:
        ComponentMeshRenderer();
        ~ComponentMeshRenderer();

        struct MONO_RENDER_API SetupParam
        {
            SetupParam() = default;

            // Vertex buffer handles
            std::vector<render_graph::ResourceHandle> vertex_buffer_handles = {};

            // Index buffer handles
            std::vector<render_graph::ResourceHandle> index_buffer_handles = {};

            // Index counts
            std::vector<uint32_t> index_counts = {};

            // Material handles
            std::vector<const render_graph::MaterialHandle*> material_handles = {};

            // Whether it casts shadows
            bool cast_shadow = true;
        };
        void Setup(SetupParam &param);

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
    };
    extern MONO_RENDER_API riaecs::ComponentRegistrar<ComponentMeshRenderer, ComponentMeshRendererMaxCount> ComponentMeshRendererID;

} // namespace mono_render