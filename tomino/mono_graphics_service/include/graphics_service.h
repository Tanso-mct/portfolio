#pragma once

#include "directx12_util/include/wrapper.h"
#include "render_graph/include/render_graph.h"
#include "render_graph/include/resource_manager.h"
#include "render_graph/include/heap_manager.h"
#include "render_graph/include/command_manager.h"
#include "render_graph/include/imgui_context_manager.h"
#include "render_graph/include/material_manager.h"
#include "render_graph/include/light_manager.h"

#include "render_graph/include/composition_pass.h"

#include "mono_service/include/service.h"
#include "mono_service/include/service_registry.h"

#include "mono_graphics_service/include/dll_config.h"

namespace mono_graphics_service
{

// The graphics service API
// It provides access to graphics service internals for commands
class MONO_GRAPHICS_SERVICE_DLL GraphicsServiceAPI :
    public mono_service::ServiceAPI
{
public:
    GraphicsServiceAPI() = default;
    virtual ~GraphicsServiceAPI() = default;

    // Get the resource adder
    virtual render_graph::ResourceAdder& GetResourceAdder() = 0;

    // Get the resource eraser
    virtual render_graph::ResourceEraser& GetResourceEraser() = 0;

    // Get the command set adder
    virtual render_graph::CommandSetAdder& GetCommandSetAdder() = 0;

    // Get the command set eraser
    virtual render_graph::CommandSetEraser& GetCommandSetEraser() = 0;

    // Get the ImGui context adder
    virtual render_graph::ImguiContextAdder& GetImguiContextAdder() = 0;

    // Get the ImGui context eraser
    virtual render_graph::ImguiContextEraser& GetImguiContextEraser() = 0;

    // Get the material adder
    virtual render_graph::MaterialAdder& GetMaterialAdder() = 0;

    // Get the material eraser
    virtual render_graph::MaterialEraser& GetMaterialEraser() = 0;

    // Get the light adder
    virtual render_graph::LightAdder& GetLightAdder() = 0;

    // Get the light eraser
    virtual render_graph::LightEraser& GetLightEraser() = 0;

    // Get the render pass by handle ID
    virtual render_graph::RenderPassBase& GetRenderPass(const render_graph::RenderPassHandleID& pass_handle_id) = 0;

    // Add a render pass to the render graph by handle ID
    virtual bool AddRenderPassToGraph(const render_graph::RenderPassHandleID& pass_handle_id) = 0;

    // Get the current back buffer index
    virtual UINT GetCurrentBackBufferIndex(const render_graph::ResourceHandle* swap_chain_handle) const = 0;

    // Get the shader resource view descriptor handle for a resource
    virtual D3D12_GPU_DESCRIPTOR_HANDLE GetTexture2DSRVHandle(
        const render_graph::ResourceHandle* resource_handle) const = 0;
};

// The number of command queue buffers for graphics service
constexpr size_t SERVICE_COMMAND_QUEUE_BUFFER_COUNT = 2;

// The graphics service handle type
class MONO_GRAPHICS_SERVICE_DLL GraphicsServiceHandle : public mono_service::ServiceHandle<GraphicsServiceHandle> {};

// The graphics service class
class MONO_GRAPHICS_SERVICE_DLL GraphicsService :
    public mono_service::Service,
    private GraphicsServiceAPI
{
public:
    GraphicsService(mono_service::ServiceThreadAffinityID thread_affinity_id);
    virtual ~GraphicsService();

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

        virtual ~SetupParam() override = default;

        UINT srv_descriptor_count = 100;
        UINT rtv_descriptor_count = 100;
        UINT dsv_descriptor_count = 100;
    };
    virtual bool Setup(mono_service::Service::SetupParam& param) override;
    virtual bool PreUpdate() override;
    virtual bool Update() override;
    virtual bool PostUpdate() override;
    virtual std::unique_ptr<mono_service::ServiceCommandList> CreateCommandList() override;
    virtual std::unique_ptr<mono_service::ServiceView> CreateView() override;

protected:
    /*******************************************************************************************************************
     * Graphics Service API
    /******************************************************************************************************************/

    GraphicsServiceAPI& GetAPI() { return *this; }
    const GraphicsServiceAPI& GetAPI() const { return *this; }

    render_graph::ResourceAdder& GetResourceAdder() override;
    render_graph::ResourceEraser& GetResourceEraser() override;

    render_graph::CommandSetAdder& GetCommandSetAdder() override;
    render_graph::CommandSetEraser& GetCommandSetEraser() override;

    render_graph::ImguiContextAdder& GetImguiContextAdder() override;
    render_graph::ImguiContextEraser& GetImguiContextEraser() override;

    render_graph::MaterialAdder& GetMaterialAdder() override;
    render_graph::MaterialEraser& GetMaterialEraser() override;

    render_graph::LightAdder& GetLightAdder() override;
    render_graph::LightEraser& GetLightEraser() override;

    render_graph::RenderPassBase& GetRenderPass(const render_graph::RenderPassHandleID& pass_handle_id) override;
    bool AddRenderPassToGraph(const render_graph::RenderPassHandleID& pass_handle_id) override;

    UINT GetCurrentBackBufferIndex(const render_graph::ResourceHandle* swap_chain_handle) const override;
    D3D12_GPU_DESCRIPTOR_HANDLE GetTexture2DSRVHandle(
        const render_graph::ResourceHandle* resource_handle) const override;

private:
    /*******************************************************************************************************************
     * DirectX 12
    /******************************************************************************************************************/

    std::unique_ptr<dx12_util::DXFactory> dx_factory_ = nullptr;
    std::unique_ptr<dx12_util::Device> dx_device_ = nullptr;
    std::unique_ptr<dx12_util::CommandQueue> dx_command_queue_ = nullptr;

    /*******************************************************************************************************************
     * Render Graph Resources
    /******************************************************************************************************************/

    std::unique_ptr<render_graph::ResourceContainer> resource_container_ = nullptr;
    std::unique_ptr<render_graph::ResourceManager> resource_manager_ = nullptr;
    std::unique_ptr<render_graph::ResourceAdder> resource_adder_ = nullptr;
    std::unique_ptr<render_graph::ResourceEraser> resource_eraser_ = nullptr;

    /*******************************************************************************************************************
     * Descriptor Heaps and Heap Manager
    /******************************************************************************************************************/

    std::unique_ptr<dx12_util::DescriptorHeap> rtv_heap_ = nullptr;
    std::unique_ptr<dx12_util::DescriptorHeapAllocator> rtv_heap_allocator_ = nullptr;
    std::unique_ptr<dx12_util::DescriptorHeap> srv_heap_ = nullptr;
    std::unique_ptr<dx12_util::DescriptorHeapAllocator> srv_heap_allocator_ = nullptr;
    std::unique_ptr<dx12_util::DescriptorHeap> dsv_heap_ = nullptr;
    std::unique_ptr<dx12_util::DescriptorHeapAllocator> dsv_heap_allocator_ = nullptr;
    std::unique_ptr<render_graph::HeapManager> heap_manager_ = nullptr;

    /*******************************************************************************************************************
     * Command
    /******************************************************************************************************************/

    std::unique_ptr<render_graph::CommandSetContainer> command_set_container_ = nullptr;
    std::unique_ptr<render_graph::CommandSetManager> command_set_manager_ = nullptr;
    std::unique_ptr<render_graph::CommandSetAdder> command_set_adder_ = nullptr;
    std::unique_ptr<render_graph::CommandSetEraser> command_set_eraser_ = nullptr;

    /*******************************************************************************************************************
     * ImGui
    /******************************************************************************************************************/

    std::unique_ptr<render_graph::ImguiContextContainer> imgui_context_container_ = nullptr;
    std::unique_ptr<render_graph::ImguiContextManager> imgui_context_manager_ = nullptr;
    std::unique_ptr<render_graph::ImguiContextAdder> imgui_context_adder_ = nullptr;
    std::unique_ptr<render_graph::ImguiContextEraser> imgui_context_eraser_ = nullptr;

    /*******************************************************************************************************************
     * Material
    /******************************************************************************************************************/

    std::unique_ptr<render_graph::MaterialTypeHandleIDGenerator> material_type_handle_id_generator_ = nullptr; // Material type handle ID generator
    std::unique_ptr<render_graph::MaterialContainer> material_container_ = nullptr; // Material container
    std::unique_ptr<render_graph::MaterialManager> material_manager_ = nullptr; // Material manager singleton
    std::unique_ptr<render_graph::MaterialAdder> material_adder_ = nullptr; // Material adder
    std::unique_ptr<render_graph::MaterialEraser> material_eraser_ = nullptr; // Material eraser

    /*******************************************************************************************************************
     * Light
    /******************************************************************************************************************/

    std::unique_ptr<render_graph::LightTypeHandleIDGenerator> light_type_handle_id_generator_ = nullptr; // Light type handle ID generator
    std::unique_ptr<render_graph::LightContainer> light_container_ = nullptr; // Light container
    std::unique_ptr<render_graph::LightManager> light_manager_ = nullptr; // Light manager singleton
    std::unique_ptr<render_graph::LightAdder> light_adder_ = nullptr; // Light adder
    std::unique_ptr<render_graph::LightEraser> light_eraser_ = nullptr; // Light eraser

    /*******************************************************************************************************************
     * Render Graph
    /******************************************************************************************************************/

    std::unique_ptr<render_graph::RenderPassIDGenerator> render_pass_id_generator = nullptr;
    std::unique_ptr<render_graph::RenderGraph> render_graph_ = nullptr;
    std::unordered_map<render_graph::RenderPassHandleID, std::unique_ptr<render_graph::RenderPassBase>> render_pass_map_;
};

} // namespace mono_graphics_service