#include "mono_render_test/pch.h"

#include "mono_file/mono_file.h"
#pragma comment(lib, "mono_file.lib")

#include "mono_asset/include/model.h"
#pragma comment(lib, "mono_asset.lib")

#include "mono_service/include/service_importer.h"
#include "mono_graphics_service/include/graphics_service.h"
#include "mono_adapter/include/service_adapter.h"

#pragma comment(lib, "riaecs.lib")

#include "mono_render/include/component_camera.h"
#include "mono_render/include/component_mesh_renderer.h"
#include "mono_render/include/ambient_light_component.h"
#include "mono_render/include/directional_light_component.h"
#include "mono_render/include/point_light_component.h"
#include "mono_render/include/ui_component.h"
#pragma comment(lib, "mono_render.lib")

#include "imgui/include/imgui.h"
#include "imgui/include/imgui_internal.h"
#include "imgui/include/imgui_impl_win32.h"
#include "imgui/include/imgui_impl_dx12.h"

using namespace DirectX;

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace mono_render_test
{

constexpr uint32_t BACK_BUFFER_COUNT = 2;

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

// Window procedure
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        return 0;

    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT menu
            return 0;
        break;

    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

} // namespace mono_render_test

TEST(Camera, Create)
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

    // Create camera component setup parameters
    mono_render::ComponentCamera::SetupParam camera_setup_param;

    // Create camera component
    std::unique_ptr<mono_render::ComponentCamera> camera_component = std::make_unique<mono_render::ComponentCamera>();

    // Setup camera component
    camera_component->Setup(camera_setup_param);

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

    // Validate camera component parameters
    EXPECT_FLOAT_EQ(camera_component->GetFovY(), camera_setup_param.fov_y);
    EXPECT_FLOAT_EQ(camera_component->GetNearZ(), camera_setup_param.nearZ);
    EXPECT_FLOAT_EQ(camera_component->GetFarZ(), camera_setup_param.farZ);
    EXPECT_FLOAT_EQ(camera_component->GetAspectRatio(), camera_setup_param.aspect_ratio);
    EXPECT_TRUE(camera_component->GetViewProjMatrixBufferHandle()->IsValid());
    EXPECT_TRUE(camera_component->GetInvViewProjMatrixBufferHandle()->IsValid());
}

TEST(MeshRenderer, Create)
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

    // Async update graphics service
    std::atomic<bool> stop_graphics_service_update = false;
    std::future<void> graphics_service_future = std::async(std::launch::async, [&]()
    {
        bool result = false;

        while (!stop_graphics_service_update.load())
        {
            service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
            {
                // Get graphics service
                mono_service::Service& service = registry.Get(mono_graphics_service::GraphicsServiceHandle::ID());

                // Pre-update
                result = service.PreUpdate();
                assert(result);

                // Update
                result = service.Update();
                assert(result);

                // Post-update
                result = service.PostUpdate();
                assert(result);
            });
        }
    });

    riaecs::ROObject<riaecs::IFileLoader> fileLoader = riaecs::gFileLoaderRegistry->Get(mono_file::FileLoaderFBXID());
    std::unique_ptr<riaecs::IFileData> fileData = fileLoader().Load("../resources/mono_asset_test/model/box.mfm");
    ASSERT_NE(fileData, nullptr);

    riaecs::ROObject<riaecs::IAssetFactory> assetFactory = riaecs::gAssetFactoryRegistry->Get(mono_asset::AssetFactoryModelID());
    std::unique_ptr<riaecs::IAssetStagingArea> stagingArea = assetFactory().Prepare();
    std::unique_ptr<riaecs::IAsset> asset = assetFactory().Create(*fileData, *stagingArea);
    assetFactory().Commit(*stagingArea);

    mono_asset::AssetModel &model_asset = static_cast<mono_asset::AssetModel&>(*asset);

    // Release data which used for loading
    fileData.reset();
    stagingArea.reset();

    // Create material handle
    render_graph::MaterialHandle material_handle = render_graph::MaterialHandle();

    // Create mesh renderer component
    std::unique_ptr<mono_render::ComponentMeshRenderer> mesh_renderer_component
        = std::make_unique<mono_render::ComponentMeshRenderer>();

    // Create mesh renderer component setup parameters
    mono_render::ComponentMeshRenderer::SetupParam mesh_renderer_setup_param;
    for (int i = 0; i < model_asset.GetVertexBufferHandles()->size(); ++i)
        mesh_renderer_setup_param.vertex_buffer_handles.push_back((*model_asset.GetVertexBufferHandles())[i]);
    for (int i = 0; i < model_asset.GetIndexBufferHandles()->size(); ++i)
        mesh_renderer_setup_param.index_buffer_handles.push_back((*model_asset.GetIndexBufferHandles())[i]);
    for (int i = 0; i < model_asset.GetIndexCounts()->size(); ++i)
        mesh_renderer_setup_param.index_counts.push_back((*model_asset.GetIndexCounts())[i]);
    mesh_renderer_setup_param.material_handles.push_back(&material_handle);
    mesh_renderer_setup_param.cast_shadow = true;

    // Setup mesh renderer component
    mesh_renderer_component->Setup(mesh_renderer_setup_param);

    // Validate vertex buffer handles
    const std::vector<render_graph::ResourceHandle>* vertex_buffer_handles
        = mesh_renderer_component->GetVertexBufferHandles();
    for (int i = 0; i < vertex_buffer_handles->size(); ++i)
        EXPECT_TRUE(vertex_buffer_handles->at(i).IsValid());

    // Validate index buffer handles
    const std::vector<render_graph::ResourceHandle>* index_buffer_handles
        = mesh_renderer_component->GetIndexBufferHandles();
    for (int i = 0; i < index_buffer_handles->size(); ++i)
        EXPECT_TRUE(index_buffer_handles->at(i).IsValid());

    // Validate index counts
    const std::vector<uint32_t>* index_counts
        = mesh_renderer_component->GetIndexCounts();
    for (int i = 0; i < index_counts->size(); ++i)
        EXPECT_GT(index_counts->at(i), 0u);

    // Finalize async graphics service update
    stop_graphics_service_update.store(true);
    graphics_service_future.get();

    // Clean up
    graphics_service_adapter.reset();
    mesh_renderer_component.reset();
    asset.reset();
    service_registry.reset();
}

TEST(AmbientLight, Create)
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

    // Create ambient light component setup parameters
    mono_render::AmbientLightComponent::SetupParam component_setup_param;
    component_setup_param.light_color = XMFLOAT4(0.1f, 0.2f, 0.3f, 1.0f);
    component_setup_param.intensity = 0.5f;

    // Create ambient light component
    std::unique_ptr<mono_render::AmbientLightComponent> component
        = std::make_unique<mono_render::AmbientLightComponent>();
    
    // Setup ambient light component
    component->Setup(component_setup_param);

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

    // Validate ambient light handle
    const render_graph::LightHandle* light_handle = component->GetLightHandle();
    EXPECT_TRUE(light_handle->IsValid());

    // Clean up
    graphics_service_adapter.reset();
    component.reset();
    service_registry.reset();
}

TEST(DirectionalLight, Create)
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

    // Create ambient light component setup parameters
    mono_render::DirectionalLightComponent::SetupParam component_setup_param;
    component_setup_param.light_color = XMFLOAT4(0.4f, 0.5f, 0.6f, 1.0f);
    component_setup_param.intensity = 1.5f;
    component_setup_param.distance = 150.0f;
    component_setup_param.ortho_width = 200.0f;
    component_setup_param.ortho_height = 200.0f;
    component_setup_param.near_z = 2.0f;
    component_setup_param.far_z = 800.0f;
    component_setup_param.shadow_map_size = 1024;

    // Create ambient light component
    std::unique_ptr<mono_render::DirectionalLightComponent> component
        = std::make_unique<mono_render::DirectionalLightComponent>();
    
    // Setup ambient light component
    component->Setup(component_setup_param);

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

    // Validate ambient light handle
    const render_graph::LightHandle* light_handle = component->GetLightHandle();
    EXPECT_TRUE(light_handle->IsValid());

    // Clean up
    graphics_service_adapter.reset();
    component.reset();
    service_registry.reset();
}

TEST(PointLight, Create)
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

    // Create point light component setup parameters
    mono_render::PointLightComponent::SetupParam component_setup_param;
    component_setup_param.light_color = XMFLOAT4(0.7f, 0.8f, 0.9f, 1.0f);
    component_setup_param.intensity = 2.0f;
    component_setup_param.range = 50.0f;

    // Create point light component
    std::unique_ptr<mono_render::PointLightComponent> component
        = std::make_unique<mono_render::PointLightComponent>();
    
    // Setup point light component
    component->Setup(component_setup_param);

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

    // Validate point light handle
    const render_graph::LightHandle* light_handle = component->GetLightHandle();
    EXPECT_TRUE(light_handle->IsValid());

    // Clean up
    graphics_service_adapter.reset();
    component.reset();
    service_registry.reset();
}

TEST(UI, Create)
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

    // Create UI component setup parameters
    mono_render::UIComponent::SetupParam component_setup_param;
    component_setup_param.ui_drawer = std::make_unique<mono_render::DefaultUIDrawer>();

    // Create UI component
    std::unique_ptr<mono_render::UIComponent> component
        = std::make_unique<mono_render::UIComponent>();
    
    // Setup UI component
    component->Setup(component_setup_param);

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

    // Validate UI drawer existence
    EXPECT_TRUE(component->HasDrawer());

    // Clean up
    graphics_service_adapter.reset();
    component.reset();
    service_registry.reset();
}

