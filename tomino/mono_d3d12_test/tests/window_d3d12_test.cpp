#include "mono_d3d12_test/pch.h"

#include "riaecs/riaecs.h"
#pragma comment(lib, "riaecs.lib")

#include "mem_alloc_fixed_block/mem_alloc_fixed_block.h"
#pragma comment(lib, "mem_alloc_fixed_block.lib")

#include "mono_d3d12/include/component_window_d3d12.h"
#include "mono_d3d12/include/window_render_bind_component.h"
#include "mono_d3d12/include/system_window_d3d12.h"
#include "mono_d3d12/include/window_message_state.h"
#pragma comment(lib, "mono_d3d12.lib")
#pragma comment(lib, "mono_input_monitor.lib")

#include "mono_service/include/service_importer.h"
#include "mono_graphics_service/include/graphics_service.h"
#include "mono_adapter/include/service_adapter.h"

#include "imgui/include/imgui.h"
#include "imgui/include/imgui_internal.h"
#include "imgui/include/imgui_impl_win32.h"
#include "imgui/include/imgui_impl_dx12.h"

#include <future>

namespace mono_d3d12_test
{

constexpr uint32_t BACK_BUFFER_COUNT = 2;
constexpr uint32_t MAX_LIGHT_COUNT = 10;

// Create ImGui context
ImGuiContext* CreateImGuiContext(HWND hwnd)
{
    bool result = false;

    // Get main monitor scale
    float main_scale 
        = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

     // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGuiContext* context = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();

    // Bake a fixed style scale. 
    // (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.ScaleAllSizes(main_scale);

    // Set initial font scale. 
    // (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)
    style.FontScaleDpi = main_scale;

    // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. 
    // This will scale fonts but _NOT_ scale sizes/padding for now.
    io.ConfigDpiScaleFonts = true;

    // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.
    io.ConfigDpiScaleViewports = true;

    // Setup Platform/Renderer backends
    result = ImGui_ImplWin32_Init(hwnd);
    if (!result)
        return nullptr;

    ImGui_ImplDX12_InitInfo init_info = {};
    init_info.Device = dx12_util::Device::GetInstance().Get();
    init_info.CommandQueue = dx12_util::CommandQueue::GetInstance().Get();
    init_info.NumFramesInFlight = BACK_BUFFER_COUNT;
    init_info.RTVFormat = render_graph::composition_pass::SWAP_CHAIN_FORMAT;
    init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;

    // Get heap manager
    render_graph::HeapManager& heap_manager = render_graph::HeapManager::GetInstance();

    heap_manager.WithUniqueLock([&](render_graph::HeapManager& heap_manager)
    {
        // Get SRV descriptor heap
        dx12_util::DescriptorHeap& srv_heap = heap_manager.GetSrvHeap();

        // Allocating SRV descriptors (for textures) is up to the application, so we provide callbacks.
        init_info.SrvDescriptorHeap = srv_heap.Get();
        init_info.SrvDescriptorAllocFn = [](
            ImGui_ImplDX12_InitInfo*, 
            D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle)
            {
                render_graph::HeapManager::GetInstance().WithUniqueLock([&](render_graph::HeapManager& heap_manager)
                {
                    dx12_util::DescriptorHeapAllocator& srv_heap_allocator = heap_manager.GetSrvHeapAllocator();
                    srv_heap_allocator.Allocate(*out_cpu_handle, *out_gpu_handle);
                });
            };
        init_info.SrvDescriptorFreeFn = [](
            ImGui_ImplDX12_InitInfo*,
            D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle)
            {
                render_graph::HeapManager::GetInstance().WithUniqueLock([&](render_graph::HeapManager& heap_manager)
                {
                    dx12_util::DescriptorHeapAllocator& srv_heap_allocator = heap_manager.GetSrvHeapAllocator();
                    srv_heap_allocator.Free(cpu_handle, gpu_handle);
                });
            };
    });

    result = ImGui_ImplDX12_Init(&init_info);
    if (!result)
        return nullptr;
    
    return context;
}

// Destroy ImGui context
void DestroyImguiContext(ImGuiContext* context)
{
    // Set current ImGui context
    ImGui::SetCurrentContext(context);

    // Shutdown ImGui
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
};

} // namespace mono_d3d12_test


TEST(WindowD3D12, Create)
{
    // Create service id generator
    std::unique_ptr<mono_service::ServiceIDGenerator> service_id_generator 
        = std::make_unique<mono_service::ServiceIDGenerator>();

    // Create service registry
    std::unique_ptr<mono_service::ServiceRegistry> service_registry = std::make_unique<mono_service::ServiceRegistry>();

    // Import graphics service
    mono_graphics_service::GraphicsService::SetupParam setup_param;
    setup_param.srv_descriptor_count = 50;
    setup_param.rtv_descriptor_count = 50;
    setup_param.dsv_descriptor_count = 50;
    mono_service::ImportService<mono_graphics_service::GraphicsService, mono_graphics_service::GraphicsServiceHandle>(
        *service_registry, 0, setup_param);

    std::unique_ptr<mono_adapter::GraphicsServiceAdapter> graphics_service_adapter = nullptr;
    service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        // Initialize service adapter singleton
        graphics_service_adapter = std::make_unique<mono_adapter::GraphicsServiceAdapter>(
            registry.Get(mono_graphics_service::GraphicsServiceHandle::ID()));
    });

    // Create ECS World
    std::unique_ptr<riaecs::IECSWorld> ecsWorld 
    = std::make_unique<riaecs::ECSWorld>(*riaecs::gComponentFactoryRegistry, *riaecs::gComponentMaxCountRegistry);
    ecsWorld->SetPoolFactory(std::make_unique<mem_alloc_fixed_block::FixedBlockPoolFactory>());
    ecsWorld->SetAllocatorFactory(std::make_unique<mem_alloc_fixed_block::FixedBlockAllocatorFactory>());
    ecsWorld->CreateWorld();

    // Create Asset Container
    std::unique_ptr<riaecs::IAssetContainer> assetCont = std::make_unique<riaecs::AssetContainer>();
    assetCont->Create(riaecs::gAssetSourceRegistry->GetCount());

    // Create System Loop Command Queue
    std::unique_ptr<riaecs::ISystemLoopCommandQueue> systemLoopCmdQueue = std::make_unique<riaecs::SystemLoopCommandQueue>();

    // Create window
    riaecs::Entity windowEntity;
    {
        // Window
        windowEntity = ecsWorld->CreateEntity();
        ecsWorld->AddComponent(windowEntity, mono_d3d12::ComponentWindowD3D12ID());

        mono_d3d12::ComponentWindowD3D12 *window
        = riaecs::GetComponent<mono_d3d12::ComponentWindowD3D12>(*ecsWorld, windowEntity, mono_d3d12::ComponentWindowD3D12ID());

        mono_d3d12::ComponentWindowD3D12::SetupParam setupParam;
        setupParam.windowName_ = L"Test Window";
        setupParam.windowClassName_ = L"TestWindowClass";
        
        window->Setup(setupParam);
        window->Create();
        window->Show();

        // Window Render Bind
        ecsWorld->AddComponent(windowEntity, mono_d3d12::WindowRenderBindComponentID());

        mono_d3d12::WindowRenderBindComponent* windowRenderBind
        = riaecs::GetComponent<mono_d3d12::WindowRenderBindComponent>(*ecsWorld, windowEntity, mono_d3d12::WindowRenderBindComponentID());

        mono_d3d12::WindowRenderBindComponent::SetupParam renderBindSetupParam;
        renderBindSetupParam.imgui_create_func = mono_d3d12_test::CreateImGuiContext;
        renderBindSetupParam.imgui_destroy_func = mono_d3d12_test::DestroyImguiContext;

        windowRenderBind->Setup(renderBindSetupParam);
        windowRenderBind->CreateBindResources(
            window->GetInfo().GetHandle(), window->GetInfo().GetWidth(), window->GetInfo().GetHeight(),
            mono_d3d12_test::BACK_BUFFER_COUNT, mono_d3d12_test::MAX_LIGHT_COUNT);
    }

    // Create window system
    mono_d3d12::SystemWindowD3D12 windowSystem;

    /*******************************************************************************************************************
     * Run the main loop to process messages
    /******************************************************************************************************************/

    bool isRunning = true;
    std::future<void> gameLoop = std::async(std::launch::async, 
        [&isRunning, &windowSystem, &ecsWorld, &assetCont, &systemLoopCmdQueue, &service_registry, &windowEntity]()
    {
        while (isRunning)
        {
            isRunning = windowSystem.Update(*ecsWorld, *assetCont, *systemLoopCmdQueue);

            bool result = false;
            service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
            {
                // Get graphics service
                mono_service::Service& service = registry.Get(mono_graphics_service::GraphicsServiceHandle::ID());

                // Pre-update
                result = service.PreUpdate();
                ASSERT_TRUE(result);

                // Update
                result = service.Update();
                ASSERT_TRUE(result);

                // Post-update
                result = service.PostUpdate();
                ASSERT_TRUE(result);
            });

            static bool validated = false;
            if (!validated)
            {
                // Get window render bind component
                mono_d3d12::WindowRenderBindComponent* component
                    = riaecs::GetComponent<mono_d3d12::WindowRenderBindComponent>(
                        *ecsWorld, windowEntity, mono_d3d12::WindowRenderBindComponentID());

                // Validate swap chain handle
                ASSERT_TRUE(component->GetSwapChainHandle()->IsValid());

                // Validate command set handles
                for (int i = 0; i < component->GetCommandSetHandles()->size(); ++i)
                    ASSERT_TRUE(component->GetCommandSetHandles()->at(i).IsValid());

                // Validate G-Buffer texture handles
                for (int i = 0; i < component->GetGBufferTextureHandles()->size(); ++i)
                {
                    const render_graph::ResourceHandles& gbuffer_handles = component->GetGBufferTextureHandles()->at(i);
                    for (int j = 0; j < gbuffer_handles.size(); ++j)
                        ASSERT_TRUE(gbuffer_handles.at(j).IsValid());
                }

                // Validate depth stencil texture handles
                for (int i = 0; i < component->GetGDepthStencilTextureHandles()->size(); ++i)
                    ASSERT_TRUE(component->GetGDepthStencilTextureHandles()->at(i).IsValid());

                // Validate full-screen triangle vertex buffer handle
                ASSERT_TRUE(component->GetFullScreenTriangleVertexBufferHandle()->IsValid());

                // Validate full-screen triangle index buffer handle
                ASSERT_TRUE(component->GetFullScreenTriangleIndexBufferHandle()->IsValid());

                // Validate ImGui render target texture handles
                for (int i = 0; i < component->GetImguiRenderTargetTextureHandles()->size(); ++i)
                    ASSERT_TRUE(component->GetImguiRenderTargetTextureHandles()->at(i).IsValid());

                // Validate Imgui context handle
                ASSERT_TRUE(component->GetImguiContextHandle()->IsValid());

                // Validate lights buffer handles
                for (int i = 0; i < component->GetLightsBufferHandles()->size(); ++i)
                    ASSERT_TRUE(component->GetLightsBufferHandles()->at(i).IsValid());

                // Validate lights upload buffer handles
                for (int i = 0; i < component->GetLightsUploadBufferHandles()->size(); ++i)
                    ASSERT_TRUE(component->GetLightsUploadBufferHandles()->at(i).IsValid());

                // Validate light config buffer handle
                ASSERT_TRUE(component->GetLightConfigBufferHandle()->IsValid());

                // Validate lighting render target texture handles
                for (int i = 0; i < component->GetLightingRenderTargetTextureHandles()->size(); ++i)
                {
                    const render_graph::ResourceHandles& lighting_handles = component->GetLightingRenderTargetTextureHandles()->at(i);
                    for (int j = 0; j < lighting_handles.size(); ++j)
                        ASSERT_TRUE(lighting_handles.at(j).IsValid());
                }

                // Validate shadow composition config buffer handle
                ASSERT_TRUE(component->GetShadowCompositionConfigBufferHandle()->IsValid());

                // Validate shadow composition render target texture handles
                for (int i = 0; i < component->GetShadowCompositionRenderTargetTextureHandles()->size(); ++i)
                {
                    const render_graph::ResourceHandles& shadow_composition_handles 
                        = component->GetShadowCompositionRenderTargetTextureHandles()->at(i);
                    for (int j = 0; j < shadow_composition_handles.size(); ++j)
                        ASSERT_TRUE(shadow_composition_handles.at(j).IsValid());
                }

                // Mark as validated
                validated = true;

                std::cout << "WindowD3D12 Test: Validation succeeded." << std::endl;
            }
        }
    });

    /*******************************************************************************************************************
     * Run the message loop to keep the window responsive
    /******************************************************************************************************************/

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (!isRunning)
            PostQuitMessage(0);
    }

    // Wait for the main loop to finish
    gameLoop.get();

    // Clean up
    graphics_service_adapter.reset();
    ecsWorld.reset();
    service_registry.reset();
}