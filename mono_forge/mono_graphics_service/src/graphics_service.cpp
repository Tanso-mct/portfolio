#include "mono_graphics_service/src/pch.h"
#include "mono_graphics_service/include/graphics_service.h"

#include "directx12_util/include/helper.h"

#include "render_graph/include/buffer_upload_pass.h"
#include "render_graph/include/imgui_pass.h"
#include "render_graph/include/texture_upload_pass.h"
#include "render_graph/include/light_upload_pass.h"

#include "render_graph/include/composition_pipeline.h"
#include "render_graph/include/composition_pass.h"

#include "render_graph/include/geometry_pass.h"
#include "render_graph/include/lambert_pipeline.h"
#include "render_graph/include/lambert_material.h"
#include "render_graph/include/phong_pipeline.h"
#include "render_graph/include/phong_material.h"

#include "render_graph/include/shadowing_pass.h"
#include "render_graph/include/shadowing_pipeline.h"

#include "render_graph/include/shadow_composition_pass.h"
#include "render_graph/include/shadow_composition_pipeline.h"

#include "render_graph/include/lighting_pass.h"
#include "render_graph/include/lighting_pipeline.h"
#include "render_graph/include/light.h"
#include "render_graph/include/directional_light.h"
#include "render_graph/include/ambient_light.h"
#include "render_graph/include/point_light.h"

#include "mono_graphics_service/include/graphics_command_list.h"
#include "mono_graphics_service/include/graphics_service_view.h"

namespace mono_graphics_service
{

GraphicsService::GraphicsService(mono_service::ServiceThreadAffinityID thread_affinity_id) :
    mono_service::Service(thread_affinity_id)
{
}

GraphicsService::~GraphicsService()
{
    bool result = false;

    // Wait for gpu to finish
    result = dx12_util::WaitForGpu(
        dx12_util::Device::GetInstance().Get(), dx12_util::CommandQueue::GetInstance().Get());
    assert(result && "Failed to wait for gpu to finish");

    /*******************************************************************************************************************
     * Material Cleanup
    /******************************************************************************************************************/

    material_eraser_.reset();
    material_adder_.reset();
    material_manager_.reset();
    material_container_.reset();

    /*******************************************************************************************************************
     * Light Cleanup
    /******************************************************************************************************************/

    light_eraser_.reset();
    light_adder_.reset();
    light_manager_.reset();
    light_container_.reset();

    /*******************************************************************************************************************
     * ImGui Cleanup
    /******************************************************************************************************************/

    imgui_context_eraser_.reset();
    imgui_context_adder_.reset();
    imgui_context_manager_.reset();
    imgui_context_container_.reset();

    /*******************************************************************************************************************
     * Command Cleanup
    /******************************************************************************************************************/

    command_set_eraser_.reset();
    command_set_adder_.reset();
    command_set_manager_.reset();
    command_set_container_.reset();

    /*******************************************************************************************************************
     * Render Graph Cleanup
    /******************************************************************************************************************/

    render_graph_.reset();
    render_pass_map_.clear();

    /*******************************************************************************************************************
     * Render Graph Resources Cleanup
    /******************************************************************************************************************/

    resource_manager_.reset();
    resource_adder_.reset();
    resource_eraser_.reset();
    resource_container_.reset();

    /*******************************************************************************************************************
     * Descriptor Heaps and Heap Manager Cleanup
    /******************************************************************************************************************/

    heap_manager_.reset();
    rtv_heap_allocator_.reset();
    rtv_heap_.reset();
    srv_heap_allocator_.reset();
    srv_heap_.reset();
    dsv_heap_allocator_.reset();
    dsv_heap_.reset();

    /*******************************************************************************************************************
     * DirectX 12 Cleanup
    /******************************************************************************************************************/

    dx_command_queue_.reset();
    dx_device_.reset();
    dx_factory_.reset();
}

bool GraphicsService::Setup(mono_service::Service::SetupParam& param)
{
    // Call base class Setup
    if (!mono_service::Service::Setup(param))
        return false;

    // Dynamic cast SetupParam
    SetupParam& graphics_setup_param = dynamic_cast<SetupParam&>(param);

    bool result = false;

    /*******************************************************************************************************************
     * Initialize DirectX 12
    /******************************************************************************************************************/

    // Create instance of DXFactory
    dx_factory_ = std::make_unique<dx12_util::DXFactory>();
    result = dx_factory_->Setup();
    if (!result) return false;

    // Create instance of Device
    dx_device_ = std::make_unique<dx12_util::Device>();
    result = dx_device_->Setup(dx_factory_->Get());
    if (!result) return false;

    // Create instance of CommandQueue
    dx_command_queue_ = std::make_unique<dx12_util::CommandQueue>();
    result = dx_command_queue_->Setup(dx_device_->Get());
    if (!result) return false;

    /*******************************************************************************************************************
     * Initialize Render Graph Resources
    /******************************************************************************************************************/

    // Create resource container
    resource_container_ = std::make_unique<render_graph::ResourceContainer>();

    // Create singleton resource manager
    resource_manager_ = std::make_unique<render_graph::ResourceManager>(*resource_container_);

    // Create resource adder
    resource_adder_ = std::make_unique<render_graph::ResourceAdder>(*resource_container_);

    // Create resource eraser
    resource_eraser_ = std::make_unique<render_graph::ResourceEraser>(*resource_container_);

    /*******************************************************************************************************************
     * Initialize Descriptor Heaps and Heap Manager
    /******************************************************************************************************************/

    // Create RTV descriptor heap
    rtv_heap_ = dx12_util::DescriptorHeap::CreateInstance<dx12_util::DescriptorHeap>(
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV, graphics_setup_param.rtv_descriptor_count, D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        dx_device_->Get());
    if (!rtv_heap_)
        return false;

    // Create RTV descriptor heap allocator
    rtv_heap_allocator_ 
        = dx12_util::DescriptorHeapAllocator::CreateInstance<dx12_util::DescriptorHeapAllocator>(
            *rtv_heap_, dx_device_->Get());
    if (!rtv_heap_allocator_)
        return false;

    // Create SRV descriptor heap
    srv_heap_ = dx12_util::DescriptorHeap::CreateInstance<dx12_util::DescriptorHeap>(
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, graphics_setup_param.srv_descriptor_count, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
        dx_device_->Get());
    if (!srv_heap_)
        return false;

    // Create SRV descriptor heap allocator
    srv_heap_allocator_ 
        = dx12_util::DescriptorHeapAllocator::CreateInstance<dx12_util::DescriptorHeapAllocator>(
            *srv_heap_, dx_device_->Get());
    if (!srv_heap_allocator_)
        return false;

    // Create DSV descriptor heap
    dsv_heap_ = dx12_util::DescriptorHeap::CreateInstance<dx12_util::DescriptorHeap>(
        D3D12_DESCRIPTOR_HEAP_TYPE_DSV, graphics_setup_param.dsv_descriptor_count, D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        dx_device_->Get());
    if (!dsv_heap_)
        return false;

    // Create DSV descriptor heap allocator
    dsv_heap_allocator_ 
        = dx12_util::DescriptorHeapAllocator::CreateInstance<dx12_util::DescriptorHeapAllocator>(
            *dsv_heap_, dx_device_->Get());
    if (!dsv_heap_allocator_)
        return false;

    // Create heap manager
    heap_manager_ = std::make_unique<render_graph::HeapManager>(
        *srv_heap_, *srv_heap_allocator_,
        *rtv_heap_, *rtv_heap_allocator_,
        *dsv_heap_, *dsv_heap_allocator_);

    /*******************************************************************************************************************
     * Initialize Command
    /******************************************************************************************************************/

    // Create command set container
    command_set_container_ = std::make_unique<render_graph::CommandSetContainer>();

    // Create command set manager
    command_set_manager_ 
        = std::make_unique<render_graph::CommandSetManager>(*command_set_container_);

    // Create command set adder
    command_set_adder_ 
        = std::make_unique<render_graph::CommandSetAdder>(*command_set_container_);

    // Create command set eraser
    command_set_eraser_ 
        = std::make_unique<render_graph::CommandSetEraser>(*command_set_container_);

    /*******************************************************************************************************************
     * Initialize ImGui
    /******************************************************************************************************************/

    // Create imgui context container
    imgui_context_container_ = std::make_unique<render_graph::ImguiContextContainer>();

    // Create imgui context manager
    imgui_context_manager_ 
        = std::make_unique<render_graph::ImguiContextManager>(*imgui_context_container_);

    // Create imgui context adder
    imgui_context_adder_ 
        = std::make_unique<render_graph::ImguiContextAdder>(*imgui_context_container_);

    // Create imgui context eraser
    imgui_context_eraser_ 
        = std::make_unique<render_graph::ImguiContextEraser>(*imgui_context_container_);

    /*******************************************************************************************************************
     * Initialize Material
    /******************************************************************************************************************/

    // Create material container
    material_container_ 
        = std::make_unique<render_graph::MaterialContainer>();

    // Create material manager singleton
    material_manager_ 
        = std::make_unique<render_graph::MaterialManager>(*material_container_);

    // Create material adder
    material_adder_ 
        = std::make_unique<render_graph::MaterialAdder>(*material_container_);

    // Create material eraser
    material_eraser_ 
        = std::make_unique<render_graph::MaterialEraser>(*material_container_);

    // Create material handle manager
    material_handle_manager_
        = std::make_unique<render_graph::MaterialHandleManager>();

    // Create material handle key generator
    material_handle_key_generator_
        = std::make_unique<render_graph::MaterialHandleKeyGenerator>();

    /*******************************************************************************************************************
     * Initialize Light
    /******************************************************************************************************************/

    // Create light type handle ID generator
    light_type_handle_id_generator_
        = std::make_unique<render_graph::LightTypeHandleIDGenerator>();

    // Create light container
    light_container_ 
        = std::make_unique<render_graph::LightContainer>();

    // Create light manager singleton
    light_manager_ 
        = std::make_unique<render_graph::LightManager>(*light_container_);

    // Create light adder
    light_adder_ 
        = std::make_unique<render_graph::LightAdder>(*light_container_);

    // Create light eraser
    light_eraser_ 
        = std::make_unique<render_graph::LightEraser>(*light_container_);

    /*******************************************************************************************************************
     * Initialization Render Graph
    /******************************************************************************************************************/

    // Create render pass ID generator
    render_pass_id_generator = std::make_unique<render_graph::RenderPassIDGenerator>();

    // Create render graph
    render_graph_ = std::make_unique<render_graph::RenderGraph>();

    // Create buffer update pass
    {
        std::unique_ptr<render_graph::BufferUploadPass> render_pass 
            = render_graph::BufferUploadPass::CreateInstance<render_graph::BufferUploadPass>();
        if (render_pass == nullptr)
            return false; // Creation failed

        // Add buffer update pass to render pass map
        assert(
            render_pass_map_.find(render_graph::BufferUploadPassHandle::ID()) == render_pass_map_.end() && 
            "BufferUploadPass already exists in render_pass_map_.");
        render_pass_map_[render_graph::BufferUploadPassHandle::ID()] = std::move(render_pass);
    }

    // Create geometry pass
    {
        render_graph::GeometryPass::PipelineMap pipelines;

        // Create Lambert pipeline
        std::unique_ptr<render_graph::LambertPipeline> lambert_pipeline
            = std::make_unique<render_graph::LambertPipeline>();
        pipelines.emplace(
            render_graph::LambertMaterialTypeHandle::ID(), std::move(lambert_pipeline));

        // Create Phong pipeline
        std::unique_ptr<render_graph::PhongPipeline> phong_pipeline
            = std::make_unique<render_graph::PhongPipeline>();
        pipelines.emplace(
            render_graph::PhongMaterialTypeHandle::ID(), std::move(phong_pipeline));

        std::unique_ptr<render_graph::GeometryPass> render_pass 
            = render_graph::GeometryPass::CreateInstance<render_graph::GeometryPass>(std::move(pipelines));
        if (render_pass == nullptr)
            return false; // Creation failed

        // Add geometry pass to render pass map
        assert(
            render_pass_map_.find(render_graph::GeometryPassHandle::ID()) == render_pass_map_.end() && 
            "GeometryPass already exists in render_pass_map_.");
        render_pass_map_[render_graph::GeometryPassHandle::ID()] = std::move(render_pass);
    }

    // Create composition pass
    {
        // Create composition pipeline
        std::unique_ptr<render_graph::CompositionPipeline> composition_pipeline
            = std::make_unique<render_graph::CompositionPipeline>();

        std::unique_ptr<render_graph::CompositionPass> render_pass 
            = render_graph::CompositionPass::CreateInstance<render_graph::CompositionPass>(
                std::move(composition_pipeline));
        if (render_pass == nullptr)
            return false; // Creation failed

        // Add composition pass to render pass map
        assert(
            render_pass_map_.find(render_graph::CompositionPassHandle::ID()) == render_pass_map_.end() && 
            "CompositionPass already exists in render_pass_map_.");
        render_pass_map_[render_graph::CompositionPassHandle::ID()] = std::move(render_pass);
    }

    // Create imgui pass
    {
        std::unique_ptr<render_graph::ImguiPass> render_pass 
            = render_graph::ImguiPass::CreateInstance<render_graph::ImguiPass>();
        if (render_pass == nullptr)
            return false; // Creation failed

        // Add imgui pass to render pass map
        assert(
            render_pass_map_.find(render_graph::ImguiPassHandle::ID()) == render_pass_map_.end() && 
            "ImguiPass already exists in render_pass_map_.");
        render_pass_map_[render_graph::ImguiPassHandle::ID()] = std::move(render_pass);
    }

    // Initialize texture upload pass
    {
        std::unique_ptr<render_graph::TextureUploadPass> render_pass 
            = render_graph::TextureUploadPass::CreateInstance<render_graph::TextureUploadPass>();
        if (render_pass == nullptr)
            return false; // Creation failed

        // Add texture upload pass to render pass map
        assert(
            render_pass_map_.find(render_graph::TextureUploadPassHandle::ID()) == render_pass_map_.end() && 
            "TextureUploadPass already exists in render_pass_map_.");
        render_pass_map_[render_graph::TextureUploadPassHandle::ID()] = std::move(render_pass);
    }

    // Initialize light upload pass
    {
        std::unique_ptr<render_graph::LightUploadPass> render_pass 
            = render_graph::LightUploadPass::CreateInstance<render_graph::LightUploadPass>();
        if (render_pass == nullptr)
            return false; // Creation failed

        // Add light upload pass to render pass map
        assert(
            render_pass_map_.find(render_graph::LightUploadPassHandle::ID()) == render_pass_map_.end() && 
            "LightUploadPass already exists in render_pass_map_.");
        render_pass_map_[render_graph::LightUploadPassHandle::ID()] = std::move(render_pass);
    }

    // Initialize lighting pass
    {
        // Create lighting pipeline
        std::unique_ptr<render_graph::LightingPipeline> lighting_pipeline
            = std::make_unique<render_graph::LightingPipeline>();

        std::unique_ptr<render_graph::LightingPass> render_pass 
            = render_graph::LightingPass::CreateInstance<render_graph::LightingPass>(std::move(lighting_pipeline));
        if (render_pass == nullptr)
            return false; // Creation failed

        // Add lighting pass to render pass map
        assert(
            render_pass_map_.find(render_graph::LightingPassHandle::ID()) == render_pass_map_.end() && 
            "LightingPass already exists in render_pass_map_.");
        render_pass_map_[render_graph::LightingPassHandle::ID()] = std::move(render_pass);
    }

    // Initialize shadowing pass
    {
        // Create shadowing pipeline
        std::unique_ptr<render_graph::ShadowingPipeline> shadowing_pipeline
            = std::make_unique<render_graph::ShadowingPipeline>();

        std::unique_ptr<render_graph::ShadowingPass> render_pass 
            = render_graph::ShadowingPass::CreateInstance<render_graph::ShadowingPass>(std::move(shadowing_pipeline));
        if (render_pass == nullptr)
            return false; // Creation failed

        // Add shadowing pass to render pass map
        assert(
            render_pass_map_.find(render_graph::ShadowingPassHandle::ID()) == render_pass_map_.end() && 
            "ShadowingPass already exists in render_pass_map_.");
        render_pass_map_[render_graph::ShadowingPassHandle::ID()] = std::move(render_pass);
    }

    // Initialize shadow composition pass
    {
        // Create shadow composition pipeline
        std::unique_ptr<render_graph::ShadowCompositionPipeline> shadow_composition_pipeline
            = std::make_unique<render_graph::ShadowCompositionPipeline>();

        std::unique_ptr<render_graph::ShadowCompositionPass> render_pass 
            = render_graph::ShadowCompositionPass::CreateInstance<render_graph::ShadowCompositionPass>(std::move(shadow_composition_pipeline));
        if (render_pass == nullptr)
            return false; // Creation failed

        // Add shadow composition pass to render pass map
        assert(
            render_pass_map_.find(render_graph::ShadowCompositionPassHandle::ID()) == render_pass_map_.end() && 
            "ShadowCompositionPass already exists in render_pass_map_.");
        render_pass_map_[render_graph::ShadowCompositionPassHandle::ID()] = std::move(render_pass);
    }

    return true; // Setup successful
}

bool GraphicsService::PreUpdate()
{
    // Call base class PreUpdate
    if (!mono_service::Service::PreUpdate())
        return false;

    return true;
}

bool GraphicsService::Update()
{
    // Begin frame update
    BeginFrame();

    // Call base class Update
    if (!mono_service::Service::Update())
        return false;

    // Define service command list vector
    using ServiceCommandLists = std::vector<std::unique_ptr<mono_service::ServiceCommandList>>;

    // Map of service command lists
    std::unordered_map<const render_graph::CommandSetHandle*, ServiceCommandLists> service_command_lists_map;

    // The no dx command service command lists
    std::vector<std::unique_ptr<mono_service::ServiceCommandList>> no_dx_command_service_command_lists;

    // The dx command lists to be executed
    std::vector<ID3D12CommandList*> dx_command_lists;

    // Swap chain handles
    // These are used to present swap chains after rendering
    std::unordered_set<const render_graph::ResourceHandle*> swap_chain_handles;

    // Collect command lists from executable command queue
    while (!GetExecutableCommandQueue().IsEmpty())
    {
        // Dequeue command list
        std::unique_ptr<mono_service::ServiceCommandList> command_list 
            = GetExecutableCommandQueue().DequeueCommandList();

        // Dynamic cast to GraphicsCommandList
        GraphicsCommandList* graphics_command_list = dynamic_cast<GraphicsCommandList*>(command_list.get());
        assert(graphics_command_list != nullptr); // Ensure the cast succeeded

        if (graphics_command_list->HasCommandSetHandle())
        {
            // Get command set handle
            const render_graph::CommandSetHandle* command_set_handle = graphics_command_list->GetCommandSetHandle();

            // Add to map
            service_command_lists_map[command_set_handle].emplace_back(std::move(command_list));
        }
        else
        {
            // Add to no dx command service command lists
            no_dx_command_service_command_lists.emplace_back(std::move(command_list));
        }

        // If the command list has a target swap chain, add it to the set
        if (graphics_command_list->HasSwapChainHandle())
            swap_chain_handles.insert(graphics_command_list->GetSwapChainHandle());
    }

    // Execute no dx command service command lists
    for (auto& command_list : no_dx_command_service_command_lists)
    {
        for (const auto& command : command_list->GetCommands())
        {
            // Execute command
            if (!command->Execute(this->GetAPI()))
                return false; // Stop update on failure
        }

        // Clear render graph because no dx commands might have modified it
        render_graph_->Clear();
    }

    // Execute service command lists
    for (auto& [command_set_handle, command_lists] : service_command_lists_map)
    {
        // Execute each command in the command lists
        for (auto& command_list : command_lists)
        {
            for (const auto& command : command_list->GetCommands())
            {
                // Execute command
                if (!command->Execute(this->GetAPI()))
                    return false; // Stop update on failure
            }
        }

        // Compile render graph
        bool result = render_graph_->Compile();
        if (!result)
            return false; // Stop update on failure

        // Get command set
        render_graph::CommandSetManager& command_set_manager = render_graph::CommandSetManager::GetInstance();
        render_graph::CommandSet* command_set = nullptr;
        command_set_manager.WithLock([&](render_graph::CommandSetManager& manager)
        {
            command_set = &manager.GetCommand(command_set_handle);
        });

        // Reset command
        command_set->ResetCommand();

        // Create render pass context
        std::unique_ptr<render_graph::RenderPassContext> context
            = std::make_unique<render_graph::RenderPassContext>(command_set->GetCommandList());

        // Execute render graph
        result = render_graph_->Execute(*context);
        if (!result)
            return false; // Stop update on failure

        // Close command
        command_set->CloseCommand();

        // Add command list to dx command lists
        dx_command_lists.push_back(command_set->GetCommandList().Get());

        // Clear render graph for next frame
        render_graph_->Clear();
    }

    // Execute all dx command lists
    if (!dx_command_lists.empty())
    {
        // Get dx command queue
        dx12_util::CommandQueue& command_queue = dx12_util::CommandQueue::GetInstance();

        // Execute command list
        command_queue.Get()->ExecuteCommandLists(
            dx_command_lists.size(), dx_command_lists.data());
    }
    
    // Present all swap chains
    render_graph::ResourceManager& resource_manager = render_graph::ResourceManager::GetInstance();
    for (const render_graph::ResourceHandle* swap_chain_handle : swap_chain_handles)
    {
        resource_manager.WithLock([&](render_graph::ResourceManager& manager)
        {
            // Create write access token for swap chain
            render_graph::ResourceAccessToken write_token;
            write_token.PermitAccess(swap_chain_handle);

            // Get swap chain resource
            dx12_util::Resource& resource 
                = manager.GetWriteResource(swap_chain_handle, write_token);

            // Cast to swap chain
            dx12_util::SwapChain& swap_chain 
                = dynamic_cast<dx12_util::SwapChain&>(resource);

            // Present swap chain
            bool result = swap_chain.Present();
            assert(result);

            // Wait for previous frame
            result = swap_chain.WaitForPreviousFrame(dx12_util::CommandQueue::GetInstance().Get());
            assert(result);
        });
    }

    // End frame update
    EndFrame();

    return true; // Update successful
}

bool GraphicsService::PostUpdate()
{
    // Call base class PostUpdate
    if (!mono_service::Service::PostUpdate())
        return false;

    return true;
}

std::unique_ptr<mono_service::ServiceCommandList> GraphicsService::CreateCommandList()
{
    return std::make_unique<GraphicsCommandList>();
}

std::unique_ptr<mono_service::ServiceView> GraphicsService::CreateView()
{
    return std::make_unique<GraphicsServiceView>(this->GetAPI());
}

render_graph::ResourceAdder& GraphicsService::GetResourceAdder()
{
    assert(IsSetup() && "GraphicsService must be set up before accessing ResourceAdder.");
    return *resource_adder_;
}

render_graph::ResourceEraser& GraphicsService::GetResourceEraser()
{
    assert(IsSetup() && "GraphicsService must be set up before accessing ResourceEraser.");
    return *resource_eraser_;
}

render_graph::CommandSetAdder& GraphicsService::GetCommandSetAdder()
{
    assert(IsSetup() && "GraphicsService must be set up before accessing CommandSetAdder.");
    return *command_set_adder_;
}

render_graph::CommandSetEraser& GraphicsService::GetCommandSetEraser()
{
    assert(IsSetup() && "GraphicsService must be set up before accessing CommandSetEraser.");
    return *command_set_eraser_;
}

render_graph::ImguiContextAdder& GraphicsService::GetImguiContextAdder()
{
    assert(IsSetup() && "GraphicsService must be set up before accessing ImGuiContextAdder.");
    return *imgui_context_adder_;
}

render_graph::ImguiContextEraser& GraphicsService::GetImguiContextEraser()
{
    assert(IsSetup() && "GraphicsService must be set up before accessing ImGuiContextEraser.");
    return *imgui_context_eraser_;
}

render_graph::MaterialAdder& GraphicsService::GetMaterialAdder()
{
    assert(IsSetup() && "GraphicsService must be set up before accessing MaterialAdder.");
    return *material_adder_;
}

render_graph::MaterialEraser& GraphicsService::GetMaterialEraser()
{
    assert(IsSetup() && "GraphicsService must be set up before accessing MaterialEraser.");
    return *material_eraser_;
}

render_graph::LightAdder& GraphicsService::GetLightAdder()
{
    assert(IsSetup() && "GraphicsService must be set up before accessing LightAdder.");
    return *light_adder_;
}

render_graph::LightEraser& GraphicsService::GetLightEraser()
{
    assert(IsSetup() && "GraphicsService must be set up before accessing LightEraser.");
    return *light_eraser_;
}

render_graph::RenderPassBase& GraphicsService::GetRenderPass(
    const render_graph::RenderPassHandleID& pass_handle_id)
{
    assert(IsSetup() && "GraphicsService must be set up before accessing RenderPass.");

    // Find render pass in map
    auto it = render_pass_map_.find(pass_handle_id);
    assert(it != render_pass_map_.end() && "RenderPassHandleID not found in render_pass_map_.");

    return *(it->second); // Return reference to render pass
}

bool GraphicsService::AddRenderPassToGraph(const render_graph::RenderPassHandleID& pass_handle_id)
{
    assert(IsSetup() && "GraphicsService must be set up before adding RenderPass to graph.");

    // Find render pass in map
    auto it = render_pass_map_.find(pass_handle_id);
    if (it == render_pass_map_.end())
        return false; // RenderPassHandleID not found

    // Add render pass to render graph
    it->second->AddToGraph(*render_graph_);

    return true; // Successfully added render pass
}

UINT mono_graphics_service::GraphicsService::GetCurrentBackBufferIndex(
    const render_graph::ResourceHandle* swap_chain_handle) const
{
    // Get current back buffer index from swap chain
    UINT frame_index = 0;
    {
        render_graph::ResourceManager& resource_manager = render_graph::ResourceManager::GetInstance();
        resource_manager.WithLock([&](render_graph::ResourceManager& manager)
        {
            // Create access token
            render_graph::ResourceAccessToken write_token;
            write_token.PermitAccess(swap_chain_handle);

            // Get swap chain for writing
            dx12_util::SwapChain* swap_chain 
                = dynamic_cast<dx12_util::SwapChain*>(&manager.GetWriteResource(swap_chain_handle, write_token));
            assert(swap_chain != nullptr); // Ensure the cast succeeded

            // Get current frame index
            frame_index = swap_chain->GetFrameIndex();
        });
    }

    return frame_index;
}

D3D12_GPU_DESCRIPTOR_HANDLE mono_graphics_service::GraphicsService::GetTexture2DSRVHandle(
    const render_graph::ResourceHandle* resource_handle) const
{
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = {};
    render_graph::ResourceManager::GetInstance().WithLock([&](render_graph::ResourceManager& manager)
    {
        // Create access token
        render_graph::ResourceAccessToken read_token;
        read_token.PermitAccess(resource_handle);

        // Get texture2D resource
        const dx12_util::Resource& texture_resource
            = render_graph::ResourceManager::GetInstance().GetReadResource(resource_handle, read_token);
        const dx12_util::Texture2D* texture
            = dynamic_cast<const dx12_util::Texture2D*>(&texture_resource);
        assert(texture != nullptr && "Failed to cast to Texture2D for texture resource.");

        // Get SRV handle
        gpu_handle = texture->GetSrvGpuHandle();
    });

    return gpu_handle;
}

const render_graph::Material* mono_graphics_service::GraphicsService::GetMaterial(
    const render_graph::MaterialHandle* material_handle) const
{
    const render_graph::Material* material = nullptr;
    render_graph::MaterialManager::GetInstance().WithLock([&](render_graph::MaterialManager& manager)
    {
        material = &manager.GetMaterial(material_handle);
    });

    return material;
}

} // namespace mono_graphics_service