#include "mono_graphics_service_test/pch.h"

using namespace DirectX;

#include "imgui/include/imgui.h"
#include "imgui/include/imgui_internal.h" // DockBuilder API
#include "imgui/include/imgui_impl_win32.h"
#include "imgui/include/imgui_impl_dx12.h"

#include "utility_header/file_loader.h"
#include "mono_forge_model/include/mfm.h"
#include "directxtex/DirectXTex.h"

#include "render_graph/include/geometry_pass.h"
#include "render_graph/include/lighting_pass.h"
#include "render_graph/include/composition_pass.h"
#include "render_graph/include/lambert_material.h"
#include "render_graph/include/phong_material.h"

#include "render_graph/include/directional_light.h"
#include "render_graph/include/ambient_light.h"
#include "render_graph/include/point_light.h"

#include "mono_service/include/service_importer.h"
#include "mono_service/include/thread_affinity.h"
#include "mono_graphics_service/include/graphics_service.h"
#include "mono_graphics_service/include/graphics_command_list.h"
#include "mono_graphics_service/include/graphics_service_view.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace mono_graphics_service_test
{

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

// Window settings
constexpr wchar_t WINDOW_CLASS_NAME[] = L"Graphics Service Test Window Class";
constexpr wchar_t WINDOW_TITLE[] = L"Graphics Service Test";
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 800;
constexpr int WINDOW_POS_X = 100;
constexpr int WINDOW_POS_Y = 100;

// Heap sizes
constexpr UINT SRV_DESCRIPTOR_COUNT = 100;
constexpr UINT RTV_DESCRIPTOR_COUNT = 20;
constexpr UINT DSV_DESCRIPTOR_COUNT = 100;

// Swap chain settings
constexpr UINT SWAP_CHAIN_BUFFER_COUNT = 2;

bool InitWindow(HWND& hwnd, WNDCLASSEXW& wc)
{
    // Register window class
    wc = 
    { 
        sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, 
        GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, WINDOW_CLASS_NAME, nullptr 
    };
    if (!::RegisterClassExW(&wc))
        return false;

    // Create window
    hwnd = ::CreateWindowW
    (
        wc.lpszClassName, WINDOW_TITLE, WS_OVERLAPPEDWINDOW, 
        WINDOW_POS_X, WINDOW_POS_Y, 
        WINDOW_WIDTH, WINDOW_HEIGHT, 
        nullptr, nullptr, wc.hInstance, nullptr
    );
    if (hwnd == nullptr)
    {
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return false;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    return true;
}

void CleanupWindow(HWND hwnd, WNDCLASSEXW& wc)
{
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
}

bool RunMessageLoop(bool& running,  std::function<bool()> frame_func)
{
    MSG msg = {};
    while (running)
    {
        while (::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);

            if (msg.message == WM_QUIT)
                return true;
        }

        if (!running)
            break; // Exit if no longer running

        // Run frame function
        running = frame_func();
    }

    return true;
}

bool ImportGraphicsService(mono_service::ServiceRegistry& registry)
{
    // Create service importer
    std::unique_ptr<mono_service::ServiceImporter> service_importer 
        = std::make_unique<mono_service::ServiceImporter>(registry);

    // Import graphics service in to registry
    std::unique_ptr<mono_graphics_service::GraphicsService> graphics_service 
        = std::make_unique<mono_graphics_service::GraphicsService>(0);

    // Create setup parameters
    mono_graphics_service::GraphicsService::SetupParam setup_param;
    setup_param.srv_descriptor_count = 50;
    setup_param.rtv_descriptor_count = 50;
    setup_param.dsv_descriptor_count = 50;

    // Import graphics service
    return service_importer->Import(
        std::move(graphics_service), mono_graphics_service::GraphicsServiceHandle::ID(), setup_param);
}

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
    init_info.NumFramesInFlight = SWAP_CHAIN_BUFFER_COUNT;
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

void DestroyImguiContext(ImGuiContext* context)
{
    // Set current ImGui context
    ImGui::SetCurrentContext(context);

    // Shutdown ImGui
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
};

bool DrawUI(ImGuiContext* context)
{
    // Set current ImGui context
    ImGui::SetCurrentContext(context);

    // Demo window
    static bool show_demo_window = true;
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // Another simple window
    static bool show_another_window = false;
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }

    return true; // Continue running
}

// Load PNG file and convert to R8G8B8A8_UNORM format if necessary
bool LoadPng(ScratchImage& out_image, TexMetadata& out_metadata, const wchar_t* file_path)
{
    HRESULT hr = E_FAIL;

    // Load normal texture
    hr = DirectX::LoadFromWICFile(file_path, DirectX::WIC_FLAGS_FORCE_SRGB, &out_metadata, out_image);
    if (FAILED(hr))
        return false;

    if (out_metadata.format != DXGI_FORMAT_R8G8B8A8_UNORM)
    {
        // Convert to R8G8B8A8_UNORM format
        DirectX::ScratchImage converted;
        hr = DirectX::Convert
        (
            out_image.GetImages(), out_image.GetImageCount(), out_metadata,
            DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::TEX_FILTER_DEFAULT, 0.0f, converted
        );
        if (FAILED(hr))
            return false;
        
        // Store converted image and format
        out_image = std::move(converted);
        out_metadata.format = DXGI_FORMAT_R8G8B8A8_UNORM;
    }

    return true; // Success
}

} // namespace mono_graphics_service_test

TEST(GraphicsService, Import)
{
    // Create service id generator
    std::unique_ptr<mono_service::ServiceIDGenerator> service_id_generator 
        = std::make_unique<mono_service::ServiceIDGenerator>();

    // Create service registry
    std::unique_ptr<mono_service::ServiceRegistry> service_registry 
        = std::make_unique<mono_service::ServiceRegistry>();

    // Import graphics service in to registry
    bool result = mono_graphics_service_test::ImportGraphicsService(*service_registry);
    ASSERT_TRUE(result);
}

TEST(GraphicsService, Execute)
{
    /*******************************************************************************************************************
     * Initialize window
    /******************************************************************************************************************/

    HWND hwnd = nullptr;
    WNDCLASSEXW wc = {};

    bool result = false;

    // Initialize window
    result = mono_graphics_service_test::InitWindow(hwnd, wc);
    ASSERT_TRUE(result);

    /*******************************************************************************************************************
     * Import graphics service
    /******************************************************************************************************************/

    // Create service id generator
    std::unique_ptr<mono_service::ServiceIDGenerator> service_id_generator 
        = std::make_unique<mono_service::ServiceIDGenerator>();

    // Create service thread affinity id generator
    std::unique_ptr<mono_service::ServiceThreadAffinityIDGenerator> thread_affinity_id_generator 
        = std::make_unique<mono_service::ServiceThreadAffinityIDGenerator>();

    // Create service registry
    std::unique_ptr<mono_service::ServiceRegistry> service_registry 
        = std::make_unique<mono_service::ServiceRegistry>();

    // Import graphics service in to registry
    result = mono_graphics_service_test::ImportGraphicsService(*service_registry);
    ASSERT_TRUE(result);

    // Get graphics service proxy from graphics service
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy = nullptr;
    service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        // Get graphics service
        mono_service::Service* service 
            = &registry.Get(mono_graphics_service::GraphicsServiceHandle::ID());

        // Create service proxy
        graphics_service_proxy = service->CreateServiceProxy();
        ASSERT_NE(graphics_service_proxy, nullptr);
    });

    /*******************************************************************************************************************
     * Add commands
    /******************************************************************************************************************/

    // Create command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list = graphics_service_proxy->CreateCommandList();
    mono_graphics_service::GraphicsCommandList& graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList&>(*command_list);

    // Create matrix buffer in graphics service
    render_graph::ResourceHandle buffer_handle = render_graph::ResourceHandle();
    graphics_command_list.CreateMatrixBuffer(&buffer_handle);

    // Submit command list to graphics service
    mono_service::ServiceProgress progress = graphics_service_proxy->SubmitCommandList(std::move(command_list));
    ASSERT_EQ(progress, graphics_service_proxy->GetProgress());

    /*******************************************************************************************************************
     * Update graphics service
    /******************************************************************************************************************/

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

        // Check progress
        ASSERT_EQ(service.GetProgress(), progress + 1);
    });

    bool is_runnning = true;
    mono_graphics_service_test::RunMessageLoop(is_runnning, []() -> bool
    {
        return true; // Exit immediately
    });

    /*******************************************************************************************************************
     * Cleanup
    /******************************************************************************************************************/

    // Cleanup service registry
    service_registry.reset();
    
    // Cleanup window
    mono_graphics_service_test::CleanupWindow(hwnd, wc);
}

TEST(GraphicsService, Rendering)
{
    // Co-initialize COM library
    HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
    ASSERT_TRUE(SUCCEEDED(hr));

    /*******************************************************************************************************************
     * Initialize window
    /******************************************************************************************************************/

    HWND hwnd = nullptr;
    WNDCLASSEXW wc = {};

    bool result = false;

    // Initialize window
    result = mono_graphics_service_test::InitWindow(hwnd, wc);
    ASSERT_TRUE(result);

    // Get client area size
    RECT client_rect = {};
    ::GetClientRect(hwnd, &client_rect);
    const UINT CLIENT_WIDTH = client_rect.right - client_rect.left;
    const UINT CLINT_HEIGHT = client_rect.bottom - client_rect.top;

    /*******************************************************************************************************************
     * Import graphics service
    /******************************************************************************************************************/

    // Create service id generator
    std::unique_ptr<mono_service::ServiceIDGenerator> service_id_generator 
        = std::make_unique<mono_service::ServiceIDGenerator>();

    // Create service thread affinity id generator
    std::unique_ptr<mono_service::ServiceThreadAffinityIDGenerator> thread_affinity_id_generator 
        = std::make_unique<mono_service::ServiceThreadAffinityIDGenerator>();

    // Create service registry
    std::unique_ptr<mono_service::ServiceRegistry> service_registry 
        = std::make_unique<mono_service::ServiceRegistry>();

    // Import graphics service in to registry
    result = mono_graphics_service_test::ImportGraphicsService(*service_registry);
    ASSERT_TRUE(result);

    // Get graphics service and its proxy from registry
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy = nullptr;
    service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        // Get graphics service
        mono_service::Service* service 
            = &registry.Get(mono_graphics_service::GraphicsServiceHandle::ID());

        // Create service proxy
        graphics_service_proxy = service->CreateServiceProxy();
        ASSERT_NE(graphics_service_proxy, nullptr);
    });

    /*******************************************************************************************************************
     * Declare resources
    /******************************************************************************************************************/

    // Swap chain
    render_graph::ResourceHandle swap_chain_handle = render_graph::ResourceHandle();

    // Command set
    render_graph::CommandSetHandles command_set_handles = render_graph::CommandSetHandles();

    // Empty texture
    render_graph::ResourceHandle empty_texture_handle = render_graph::ResourceHandle();
    render_graph::ResourceHandle empty_texture_upload_buffer_handle = render_graph::ResourceHandle();
    constexpr UINT EMPTY_TEXTURE_WIDTH = 1;
    constexpr UINT EMPTY_TEXTURE_HEIGHT = 1;
    constexpr DXGI_FORMAT EMPTY_TEXTURE_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

    // Marble bust albedo texture
    render_graph::ResourceHandle marble_bust_albedo_texture_handle = render_graph::ResourceHandle();
    render_graph::ResourceHandle marble_bust_albedo_texture_upload_buffer_handle = render_graph::ResourceHandle();
    ScratchImage marble_bust_albedo_image;
    TexMetadata marble_bust_albedo_metadata;
    constexpr const wchar_t* MARBLE_BUST_ALBEDO_TEXTURE_PATH 
        = L"../resources/render_graph_test/marble_bust_01_1k/marble_bust_01_diff_1k.png";

    // Marble bust normal texture
    render_graph::ResourceHandle marble_bust_normal_texture_handle = render_graph::ResourceHandle();
    render_graph::ResourceHandle marble_bust_normal_texture_upload_buffer_handle = render_graph::ResourceHandle();
    ScratchImage marble_bust_normal_image;
    TexMetadata marble_bust_normal_metadata;
    constexpr const wchar_t* MARBLE_BUST_NORMAL_TEXTURE_PATH 
        = L"../resources/render_graph_test/marble_bust_01_1k/marble_bust_01_nor_dx_1k.png";

    // Marble bust ambient occlusion texture
    render_graph::ResourceHandle marble_bust_ao_texture_handle = render_graph::ResourceHandle();
    render_graph::ResourceHandle marble_bust_ao_texture_upload_buffer_handle = render_graph::ResourceHandle();
    ScratchImage marble_bust_ao_image;
    TexMetadata marble_bust_ao_metadata;
    constexpr const wchar_t* MARBLE_BUST_AO_TEXTURE_PATH 
        = L"../resources/render_graph_test/marble_bust_01_1k/marble_bust_01_ao_1k.png";

    // Marble bust roughness texture
    render_graph::ResourceHandle marble_bust_roughness_texture_handle = render_graph::ResourceHandle();
    render_graph::ResourceHandle marble_bust_roughness_texture_upload_buffer_handle = render_graph::ResourceHandle();
    ScratchImage marble_bust_roughness_image;
    TexMetadata marble_bust_roughness_metadata;
    constexpr const wchar_t* MARBLE_BUST_ROUGHNESS_TEXTURE_PATH 
        = L"../resources/render_graph_test/marble_bust_01_1k/marble_bust_01_rough_1k.png";

    // Textures for Geometry Pass
    std::vector<render_graph::ResourceHandles> gbuffer_texture_handles;

    // Depth stencil texture for Geometry Pass
    render_graph::ResourceHandles depth_stencil_texture_handles = render_graph::ResourceHandles();

    // Full screen triangle vertex buffer for Composition Pass
    render_graph::ResourceHandle full_screen_triangle_vertex_buffer_handle = render_graph::ResourceHandle();

    // Full screen triangle index buffer for Composition Pass
    render_graph::ResourceHandle full_screen_triangle_index_buffer_handle = render_graph::ResourceHandle();

    // Imgui render target textures
    render_graph::ResourceHandles imgui_render_target_texture_handles = render_graph::ResourceHandles();

    // Imgui context
    render_graph::ImguiContextHandle imgui_context_handle = render_graph::ImguiContextHandle();

    // Lights buffer handle
    std::vector<render_graph::ResourceHandle> light_buffer_handles;
    std::vector<render_graph::ResourceHandle> light_buffer_upload_buffer_handles;
    constexpr uint32_t MAX_LIGHT_COUNT = 10;

    // Light configuration buffer handle
    render_graph::ResourceHandle light_config_buffer_handle = render_graph::ResourceHandle();

    // Render target texture handles for lighting passe
    std::vector<render_graph::ResourceHandles> lighting_render_target_texture_handles;

    // Shadow composition config buffer handle
    render_graph::ResourceHandle shadow_composition_config_buffer_handle = render_graph::ResourceHandle();

    // Render target texture handle for shadow composition pass
    std::vector<render_graph::ResourceHandles> shadow_composition_render_target_texture_handles;

    // Camera
    XMMATRIX camera_world_matrix = XMMatrixIdentity();
    constexpr XMFLOAT3 CAMERA_POSITION = XMFLOAT3(0.0f, 0.0f, -2.0f);
    constexpr XMFLOAT3 CAMERA_ROTATION = XMFLOAT3(0.0f, 0.0f, 0.0f); // Euler angles
    constexpr float CAMERA_FOV_Y_DEGREE = 60.0f;
    constexpr float CAMERA_NEAR_Z = 0.1f;
    constexpr float CAMERA_FAR_Z = 100.0f;
    const float CAMERA_ASPECT_RATIO = static_cast<float>(CLIENT_WIDTH) / static_cast<float>(CLINT_HEIGHT);

    // Camera view-projection matrix buffer handle
    render_graph::ResourceHandle view_proj_mat_buffer_handle = render_graph::ResourceHandle();
    XMMATRIX view_proj_matrix = XMMatrixIdentity();

    // Camera inverse View-projection matrix buffer handle
    render_graph::ResourceHandle inv_view_proj_mat_buffer_handle = render_graph::ResourceHandle();
    XMMATRIX inv_view_proj_matrix = XMMatrixIdentity();

    // Marble bust phong material handle
    render_graph::MaterialHandle marble_bust_phong_material_handle = render_graph::MaterialHandle();
    constexpr XMFLOAT4 MARBLE_BUST_BASE_COLOR = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    constexpr XMFLOAT4 MARBLE_BUST_EMISSION_COLOR = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

    // Floor lambert material handle
    render_graph::MaterialHandle floor_lambert_material_handle = render_graph::MaterialHandle();
    constexpr XMFLOAT4 FLOOR_BASE_COLOR = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    constexpr XMFLOAT4 FLOOR_EMISSION_COLOR = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

    // Shadow configuration
    constexpr float SHADOW_BIAS = 0.005f;
    constexpr float SHADOW_SLOPE_SCALED_BIAS = 0.001f;
    constexpr float SHADOW_SLOPE_BIAS_EXPONENT = 100.0f;
    constexpr float SHADOW_INTENSITY = 0.2f;

    // Directional light handle
    render_graph::LightHandle directional_light_handle = render_graph::LightHandle();
    constexpr XMFLOAT4 DIRECTIONAL_LIGHT_COLOR = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    constexpr float DIRECTIONAL_LIGHT_INTENSITY = 1.0f;
    constexpr float DIRECTIONAL_LIGHT_DISTANCE = 10.0f;
    constexpr float DIRECTIONAL_LIGHT_ORTHOGRAPHIC_WIDTH = 10.0f;
    constexpr float DIRECTIONAL_LIGHT_ORTHOGRAPHIC_HEIGHT = 10.0f;
    constexpr float DIRECTIONAL_LIGHT_NEAR_Z = 0.1f;
    constexpr float DIRECTIONAL_LIGHT_FAR_Z = 100.0f;
    constexpr uint32_t DIRECTIONAL_LIGHT_SHADOW_MAP_SIZE = 2048;
    constexpr XMFLOAT3 DIRECTIONAL_LIGHT_POSITION = XMFLOAT3(0.0f, 0.0f, 0.0f);
    constexpr XMFLOAT3 DIRECTIONAL_LIGHT_ROTATION = XMFLOAT3(60.0f, 90.0f, 0.0f); // Euler angles

    // Ambient light handle
    render_graph::LightHandle ambient_light_handle = render_graph::LightHandle();
    constexpr XMFLOAT4 AMBIENT_LIGHT_COLOR = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    constexpr float AMBIENT_LIGHT_INTENSITY = 0.2f;
    constexpr XMFLOAT3 AMBIENT_LIGHT_POSITION = XMFLOAT3(0.0f, 0.0f, 0.0f);
    constexpr XMFLOAT3 AMBIENT_LIGHT_ROTATION = XMFLOAT3(0.0f, 0.0f, 0.0f); // Euler angles

    // Point light handle
    render_graph::LightHandle point_light_handle = render_graph::LightHandle();
    constexpr XMFLOAT4 POINT_LIGHT_COLOR = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    constexpr float POINT_LIGHT_INTENSITY = 1.0f;
    constexpr float POINT_LIGHT_RANGE = 5.0f;
    constexpr XMFLOAT3 POINT_LIGHT_POSITION = XMFLOAT3(2.0f, 2.0f, -2.0f);
    constexpr XMFLOAT3 POINT_LIGHT_ROTATION = XMFLOAT3(0.0f, 0.0f, 0.0f); // Euler angles

    // Marble bust object handles
    constexpr const char* MARBLE_BUST_MODEL_PATH 
        = "../resources/render_graph_test/marble_bust_01_1k/marble_bust_01_1k.mfm";
    std::vector<render_graph::ResourceHandle> marble_bust_vertex_buffer_handle = std::vector<render_graph::ResourceHandle>();
    std::vector<render_graph::ResourceHandle> marble_bust_index_buffer_handle = std::vector<render_graph::ResourceHandle>();
    std::vector<uint32_t> marble_bust_index_count = {};
    render_graph::ResourceHandle marble_bust_world_buffer_handle = render_graph::ResourceHandle();
    XMFLOAT3 marble_bust_position = XMFLOAT3(0.0f, -0.8f, 0.0f);
    XMFLOAT3 marble_bust_rotation = XMFLOAT3(0.0f, 180.0f, 0.0f); // Euler angles
    XMFLOAT3 marble_bust_scale = XMFLOAT3(0.03f, 0.03f, 0.03f);
    render_graph::geometry_pass::WorldBuffer marble_bust_world_buffer = render_graph::geometry_pass::WorldBuffer();
    std::unique_ptr<mono_forge_model::MFM> marble_bust_model = nullptr;

    // Floor object handles
    constexpr const char* FLOOR_MODEL_PATH 
        = "../resources/render_graph_test/floor/floor.mfm";
    std::vector<render_graph::ResourceHandle> floor_vertex_buffer_handle = std::vector<render_graph::ResourceHandle>();
    std::vector<render_graph::ResourceHandle> floor_index_buffer_handle = std::vector<render_graph::ResourceHandle>();
    std::vector<uint32_t> floor_index_count = {};
    render_graph::ResourceHandle floor_world_buffer_handle = render_graph::ResourceHandle();
    XMFLOAT3 floor_position = XMFLOAT3(0.0f, -1.0f, 0.0f);
    XMFLOAT3 floor_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f); // Euler angles
    XMFLOAT3 floor_scale = XMFLOAT3(10.0f, 1.0f, 10.0f);
    render_graph::geometry_pass::WorldBuffer floor_world_buffer = render_graph::geometry_pass::WorldBuffer();
    std::unique_ptr<mono_forge_model::MFM> floor_model = nullptr;

    /*******************************************************************************************************************
     * Load files
    /******************************************************************************************************************/

    // Load marble bust albedo texture file
    result = mono_graphics_service_test::LoadPng(
        marble_bust_albedo_image, marble_bust_albedo_metadata, MARBLE_BUST_ALBEDO_TEXTURE_PATH);
    ASSERT_TRUE(result);

    // Load marble bust normal texture file
    result = mono_graphics_service_test::LoadPng(
        marble_bust_normal_image, marble_bust_normal_metadata, MARBLE_BUST_NORMAL_TEXTURE_PATH);
    ASSERT_TRUE(result);

    // Load marble bust ambient occlusion texture file
    result = mono_graphics_service_test::LoadPng(
        marble_bust_ao_image, marble_bust_ao_metadata, MARBLE_BUST_AO_TEXTURE_PATH);
    ASSERT_TRUE(result);

    // Load marble bust roughness texture file
    result = mono_graphics_service_test::LoadPng(
        marble_bust_roughness_image, marble_bust_roughness_metadata, MARBLE_BUST_ROUGHNESS_TEXTURE_PATH);
    ASSERT_TRUE(result);

    {
        // Load marble bust model file
        fpos_t marble_bust_file_size = 0;
        std::unique_ptr<uint8_t[]> marble_bust_file_data 
            = utility_header::LoadFile(MARBLE_BUST_MODEL_PATH, marble_bust_file_size);
        ASSERT_NE(marble_bust_file_data, nullptr);

        // Create mfm object
        marble_bust_model = std::make_unique<mono_forge_model::MFM>(
            std::move(marble_bust_file_data), marble_bust_file_size);
    }

    {
        // Load floor model file
        fpos_t floor_file_size = 0;
        std::unique_ptr<uint8_t[]> floor_file_data 
            = utility_header::LoadFile(FLOOR_MODEL_PATH, floor_file_size);
        ASSERT_NE(floor_file_data, nullptr);

        // Create mfm object
        floor_model = std::make_unique<mono_forge_model::MFM>(std::move(floor_file_data), floor_file_size);
    }

    /*******************************************************************************************************************
     * Create window resource in graphics service
    /******************************************************************************************************************/

    mono_service::ServiceProgress progress = 0;

    {
        // Create command list
        std::unique_ptr<mono_service::ServiceCommandList> command_list
            = graphics_service_proxy->CreateCommandList();
        mono_graphics_service::GraphicsCommandList& graphics_command_list
            = dynamic_cast<mono_graphics_service::GraphicsCommandList&>(*command_list);

        // Create swap chain in graphics service
        graphics_command_list.CreateSwapChain(
            &swap_chain_handle, hwnd, CLIENT_WIDTH, CLINT_HEIGHT, mono_graphics_service_test::SWAP_CHAIN_BUFFER_COUNT);

        // Create command set in graphics service
        graphics_command_list.CreateCommandSet(
            &command_set_handles, mono_graphics_service_test::SWAP_CHAIN_BUFFER_COUNT);

        // Create textures for Geometry Pass in graphics
        graphics_command_list.CreateTexture2DsForGeometryPass(
            &gbuffer_texture_handles, CLIENT_WIDTH, CLINT_HEIGHT, mono_graphics_service_test::SWAP_CHAIN_BUFFER_COUNT);

        // Create depth stencil texture for Geometry Pass in graphics service
        graphics_command_list.CreateDepthStencilForGeometryPass(
            &depth_stencil_texture_handles, CLIENT_WIDTH, CLINT_HEIGHT, mono_graphics_service_test::SWAP_CHAIN_BUFFER_COUNT);

        // Create full-screen triangle vertex buffer in graphics service
        graphics_command_list.CreateFullScreenTriangleVertexBufferForComposition(
            &full_screen_triangle_vertex_buffer_handle);

        // Create full-screen triangle index buffer in graphics service
        graphics_command_list.CreateFullScreenTriangleIndexBufferForComposition(
            &full_screen_triangle_index_buffer_handle);

        // Create Imgui render target textures in graphics service
        graphics_command_list.CreateImguiRenderTargetTextures(
            &imgui_render_target_texture_handles, 
            CLIENT_WIDTH, CLINT_HEIGHT, mono_graphics_service_test::SWAP_CHAIN_BUFFER_COUNT);

        // Create Imgui context in graphics service
        graphics_command_list.CreateImguiContext(
            &imgui_context_handle, hwnd,
            mono_graphics_service_test::CreateImGuiContext, mono_graphics_service_test::DestroyImguiContext);

        // Create lights buffer in graphics service
        graphics_command_list.CreateLightsBuffer(
            &light_buffer_handles, &light_buffer_upload_buffer_handles, 
            mono_graphics_service_test::SWAP_CHAIN_BUFFER_COUNT, MAX_LIGHT_COUNT);

        // Create light configuration buffer in graphics service
        graphics_command_list.CreateLightConfigBuffer(&light_config_buffer_handle);

        // Create render target textures for lighting passes in graphics service
        graphics_command_list.CreateRenderTargetTexturesForLightingPasse(
            &lighting_render_target_texture_handles, 
            CLIENT_WIDTH, CLINT_HEIGHT, mono_graphics_service_test::SWAP_CHAIN_BUFFER_COUNT);

        // Create shadow composition config buffer in graphics service
        graphics_command_list.CreateShadowCompositionConfigBuffer(&shadow_composition_config_buffer_handle);

        // Create render target texture for shadow composition pass in graphics service
        graphics_command_list.CreateRenderTargetTexturesForShadowCompositionPass(
            &shadow_composition_render_target_texture_handles,
            CLIENT_WIDTH, CLINT_HEIGHT, mono_graphics_service_test::SWAP_CHAIN_BUFFER_COUNT);

        // Submit command list to graphics service
        progress = graphics_service_proxy->SubmitCommandList(std::move(command_list));
    }

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

    // Check progress
    service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        // Get graphics service
        mono_service::Service& service = registry.Get(mono_graphics_service::GraphicsServiceHandle::ID());
        mono_graphics_service::GraphicsService& graphics_service
            = dynamic_cast<mono_graphics_service::GraphicsService&>(service);

        // Check progress
        // Progress should have advanced by 1 after one update cycle
        ASSERT_EQ(graphics_service.GetProgress(), progress + 1);
    });

    /*******************************************************************************************************************
     * Create asset resources in graphics service
    /******************************************************************************************************************/

    {
        // Create command list
        std::unique_ptr<mono_service::ServiceCommandList> command_list
            = graphics_service_proxy->CreateCommandList();
        mono_graphics_service::GraphicsCommandList& graphics_command_list
            = dynamic_cast<mono_graphics_service::GraphicsCommandList&>(*command_list);

        // Create empty texture in graphics service
        graphics_command_list.CreateShaderResourceTexture2D(
            &empty_texture_handle, &empty_texture_upload_buffer_handle, 
            EMPTY_TEXTURE_WIDTH, EMPTY_TEXTURE_HEIGHT, EMPTY_TEXTURE_FORMAT);

        // Create marble bust albedo texture in graphics service
        graphics_command_list.CreateShaderResourceTexture2D(
            &marble_bust_albedo_texture_handle, &marble_bust_albedo_texture_upload_buffer_handle,
            static_cast<UINT>(marble_bust_albedo_metadata.width), static_cast<UINT>(marble_bust_albedo_metadata.height),
            marble_bust_albedo_metadata.format, marble_bust_albedo_image.GetPixels());

        // Create marble bust normal texture in graphics service
        graphics_command_list.CreateShaderResourceTexture2D(
            &marble_bust_normal_texture_handle, &marble_bust_normal_texture_upload_buffer_handle,
            static_cast<UINT>(marble_bust_normal_metadata.width), static_cast<UINT>(marble_bust_normal_metadata.height),
            marble_bust_normal_metadata.format, marble_bust_normal_image.GetPixels());

        // Create marble bust ambient occlusion texture in graphics service
        graphics_command_list.CreateShaderResourceTexture2D(
            &marble_bust_ao_texture_handle, &marble_bust_ao_texture_upload_buffer_handle,
            static_cast<UINT>(marble_bust_ao_metadata.width), static_cast<UINT>(marble_bust_ao_metadata.height),
            marble_bust_ao_metadata.format, marble_bust_ao_image.GetPixels());

        // Create marble bust roughness texture in graphics service
        graphics_command_list.CreateShaderResourceTexture2D(
            &marble_bust_roughness_texture_handle, &marble_bust_roughness_texture_upload_buffer_handle,
            static_cast<UINT>(marble_bust_roughness_metadata.width), static_cast<UINT>(marble_bust_roughness_metadata.height),
            marble_bust_roughness_metadata.format, marble_bust_roughness_image.GetPixels());

        // Submit command list to graphics service
        progress = graphics_service_proxy->SubmitCommandList(std::move(command_list));
    }

    /*******************************************************************************************************************
     * Create run-time resources in graphics service
    /******************************************************************************************************************/

    {
        // Create command list
        std::unique_ptr<mono_service::ServiceCommandList> command_list
            = graphics_service_proxy->CreateCommandList();
        mono_graphics_service::GraphicsCommandList& graphics_command_list
            = dynamic_cast<mono_graphics_service::GraphicsCommandList&>(*command_list);

        // Create camera view-projection matrix buffer in graphics service
        graphics_command_list.CreateMatrixBuffer(&view_proj_mat_buffer_handle);

        // Create camera inverse view-projection matrix buffer in graphics service
        graphics_command_list.CreateMatrixBuffer(&inv_view_proj_mat_buffer_handle);

        {
            // Create marble bust phong material setup parameters
            std::unique_ptr<render_graph::PhongMaterial::SetupParam> setup_param 
                = std::make_unique<render_graph::PhongMaterial::SetupParam>();
            setup_param->base_color = MARBLE_BUST_BASE_COLOR;
            setup_param->albedo_source = render_graph::PhongMaterial::ALBEDO_SOURCE_TEXTURE;
            setup_param->normal_source = render_graph::PhongMaterial::NORMAL_SOURCE_TEXTURE;
            setup_param->ao_source = render_graph::PhongMaterial::AO_SOURCE_TEXTURE;
            setup_param->specular_source = render_graph::PhongMaterial::SPECULAR_SOURCE_NONE;
            setup_param->roughness_source = render_graph::PhongMaterial::ROUGHNESS_SOURCE_TEXTURE;
            setup_param->roughness_value = 1.0f;
            setup_param->metalness_source = render_graph::PhongMaterial::METALNESS_SOURCE_VALUE;
            setup_param->metalness_value = 0.0f;
            setup_param->emission_source = render_graph::PhongMaterial::EMISSION_SOURCE_COLOR;
            setup_param->emission_color = MARBLE_BUST_EMISSION_COLOR;

            setup_param->albedo_texture_handle = &marble_bust_albedo_texture_handle;
            setup_param->normal_texture_handle = &marble_bust_normal_texture_handle;
            setup_param->ao_texture_handle = &marble_bust_ao_texture_handle;
            setup_param->specular_texture_handle = &empty_texture_handle;
            setup_param->roughness_texture_handle = &marble_bust_roughness_texture_handle;
            setup_param->metalness_texture_handle = &empty_texture_handle;
            setup_param->emission_texture_handle = &empty_texture_handle;

            // Create marble bust phong material in graphics service
            graphics_command_list.CreateMaterial<render_graph::PhongMaterial>(
                &marble_bust_phong_material_handle, std::move(setup_param));
        }

        {
            // Create floor lambert material setup parameters
            std::unique_ptr<render_graph::LambertMaterial::SetupParam> setup_param
                = std::make_unique<render_graph::LambertMaterial::SetupParam>();
            setup_param->albedo_source = render_graph::LambertMaterial::ALBEDO_SOURCE_BASE_COLOR;
            setup_param->normal_source = render_graph::LambertMaterial::NORMAL_SOURCE_VERTEX;
            setup_param->ao_source = render_graph::LambertMaterial::AO_SOURCE_NONE;
            setup_param->emission_source = render_graph::LambertMaterial::EMISSION_SOURCE_NONE;
            setup_param->emission_color = FLOOR_EMISSION_COLOR;

            setup_param->albedo_texture_handle = &empty_texture_handle;
            setup_param->normal_texture_handle = &empty_texture_handle;
            setup_param->ao_texture_handle = &empty_texture_handle;
            setup_param->emission_texture_handle = &empty_texture_handle;
            
            // Create floor lambert material in graphics service
            graphics_command_list.CreateMaterial<render_graph::LambertMaterial>(
                &floor_lambert_material_handle, std::move(setup_param));
        }

        {
            // Create directional light setup parameters
            std::unique_ptr<render_graph::DirectionalLight::SetupParam> setup_param
                = std::make_unique<render_graph::DirectionalLight::SetupParam>();
            setup_param->color = DIRECTIONAL_LIGHT_COLOR;
            setup_param->intensity = DIRECTIONAL_LIGHT_INTENSITY;
            setup_param->distance = DIRECTIONAL_LIGHT_DISTANCE;
            setup_param->ortho_width = DIRECTIONAL_LIGHT_ORTHOGRAPHIC_WIDTH;
            setup_param->ortho_height = DIRECTIONAL_LIGHT_ORTHOGRAPHIC_HEIGHT;
            setup_param->near_z = DIRECTIONAL_LIGHT_NEAR_Z;
            setup_param->far_z = DIRECTIONAL_LIGHT_FAR_Z;
            setup_param->shadow_map_size = DIRECTIONAL_LIGHT_SHADOW_MAP_SIZE;

            // Create directional light in graphics service
            graphics_command_list.CreateDirectionalLight(
                &directional_light_handle, std::move(setup_param), mono_graphics_service_test::SWAP_CHAIN_BUFFER_COUNT);
        }

        {
            // Create ambient light setup parameters
            std::unique_ptr<render_graph::AmbientLight::SetupParam> setup_param
                = std::make_unique<render_graph::AmbientLight::SetupParam>();
            setup_param->color = AMBIENT_LIGHT_COLOR;
            setup_param->intensity = AMBIENT_LIGHT_INTENSITY;

            // Create ambient light in graphics service
            graphics_command_list.CreateAmbientLight(
                &ambient_light_handle, std::move(setup_param), mono_graphics_service_test::SWAP_CHAIN_BUFFER_COUNT);
        }

        {
            // Create point light setup parameters
            std::unique_ptr<render_graph::PointLight::SetupParam> setup_param
                = std::make_unique<render_graph::PointLight::SetupParam>();
            setup_param->color = POINT_LIGHT_COLOR;
            setup_param->intensity = POINT_LIGHT_INTENSITY;
            setup_param->range = POINT_LIGHT_RANGE;

            // Create point light in graphics service
            graphics_command_list.CreatePointLight(
                &point_light_handle, std::move(setup_param), mono_graphics_service_test::SWAP_CHAIN_BUFFER_COUNT);
        }

        // Resize marble bust model handles vectors
        marble_bust_vertex_buffer_handle.resize(marble_bust_model->GetMeshHeader()->material_count);
        marble_bust_index_buffer_handle.resize(marble_bust_model->GetMeshHeader()->material_count);
        marble_bust_index_count.resize(marble_bust_model->GetMeshHeader()->material_count);
        for (uint32_t material_index = 0; material_index < marble_bust_model->GetMeshHeader()->material_count; ++material_index)
        {
            // Get mesh node
            const mono_forge_model::MFMMeshNode* mesh_node = marble_bust_model->GetMeshNode(material_index);

            // Get material name
            const char* material_name = marble_bust_model->GetMaterialName(material_index);

            // Cast vertex data type to vertex format
            const render_graph::geometry_pass::Vertex* vertex_data 
                = reinterpret_cast<const render_graph::geometry_pass::Vertex*>(
                    marble_bust_model->GetVertexData(material_index));

            // Create vertex buffer for the material in graphics service
            graphics_command_list.CreateVertexBuffer(
                &marble_bust_vertex_buffer_handle[material_index], mesh_node->vertex_count, vertex_data);

            // Cast index data type to uint32_t
            const uint32_t* index_data 
                = reinterpret_cast<const uint32_t*>(marble_bust_model->GetIndexData(material_index));

            // Create index buffer for the material in graphics service
            graphics_command_list.CreateIndexBuffer(
                &marble_bust_index_buffer_handle[material_index], mesh_node->index_count, index_data);

            // Store index count
            marble_bust_index_count[material_index] = mesh_node->index_count;
        }

        // Create marble bust world matrix buffer in graphics service
        graphics_command_list.CreateWorldBufferForGeometryPass(&marble_bust_world_buffer_handle);

        // Resize floor model handles vectors
        floor_vertex_buffer_handle.resize(floor_model->GetMeshHeader()->material_count);
        floor_index_buffer_handle.resize(floor_model->GetMeshHeader()->material_count);
        floor_index_count.resize(floor_model->GetMeshHeader()->material_count);
        for (uint32_t material_index = 0; material_index < floor_model->GetMeshHeader()->material_count; ++material_index)
        {
            // Get mesh node
            const mono_forge_model::MFMMeshNode* mesh_node = floor_model->GetMeshNode(material_index);

            // Get material name
            const char* material_name = floor_model->GetMaterialName(material_index);

            // Cast vertex data type to vertex format
            const render_graph::geometry_pass::Vertex* vertex_data 
                = reinterpret_cast<const render_graph::geometry_pass::Vertex*>(
                    floor_model->GetVertexData(material_index));

            // Create vertex buffer for the material in graphics service
            graphics_command_list.CreateVertexBuffer(
                &floor_vertex_buffer_handle[material_index], mesh_node->vertex_count, vertex_data);

            // Cast index data type to uint32_t
            const uint32_t* index_data 
                = reinterpret_cast<const uint32_t*>(floor_model->GetIndexData(material_index));

            // Create index buffer for the material in graphics service
            graphics_command_list.CreateIndexBuffer(
                &floor_index_buffer_handle[material_index], mesh_node->index_count, index_data);

            // Store index count
            floor_index_count[material_index] = mesh_node->index_count;
        }

        // Create floor world matrix buffer in graphics service
        graphics_command_list.CreateWorldBufferForGeometryPass(&floor_world_buffer_handle);

        // Submit command list to graphics service
        progress = graphics_service_proxy->SubmitCommandList(std::move(command_list));
    }

    /*******************************************************************************************************************
     * Resource update preparation
    /******************************************************************************************************************/

    // Prepare camera world matrix
    {
        // Calculate world matrix
        XMVECTOR translation_vec = XMLoadFloat3(&CAMERA_POSITION);
        XMVECTOR rotation_vec = XMQuaternionRotationRollPitchYaw(
            XMConvertToRadians(CAMERA_ROTATION.x),
            XMConvertToRadians(CAMERA_ROTATION.y),
            XMConvertToRadians(CAMERA_ROTATION.z));

        XMMATRIX translation_matrix = XMMatrixTranslationFromVector(translation_vec);
        XMMATRIX rotation_matrix = XMMatrixRotationQuaternion(rotation_vec);

        camera_world_matrix = rotation_matrix * translation_matrix;

        // Transpose matrix for HLSL
        camera_world_matrix = XMMatrixTranspose(camera_world_matrix);
    }

    // Prepare camera view and projection matrix data
    {
        // Calculate view matrix
        XMVECTOR up_vec = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        XMVECTOR forward_vec = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
        XMVECTOR rot_vec = XMQuaternionRotationRollPitchYaw(
            XMConvertToRadians(CAMERA_ROTATION.x),
            XMConvertToRadians(CAMERA_ROTATION.y),
            XMConvertToRadians(CAMERA_ROTATION.z));
        XMVECTOR rotated_forward_vec = XMVector3Rotate(forward_vec, rot_vec);
        XMVECTOR rotated_up_vec = XMVector3Rotate(up_vec, rot_vec);
        XMVECTOR position_vec = XMLoadFloat3(&CAMERA_POSITION);

        XMMATRIX view_matrix = XMMatrixLookAtLH(
            position_vec, XMVectorAdd(position_vec, rotated_forward_vec), rotated_up_vec);

        // Calculate projection matrix
        XMMATRIX projection_matrix = XMMatrixPerspectiveFovLH(
            XMConvertToRadians(CAMERA_FOV_Y_DEGREE), CAMERA_ASPECT_RATIO, CAMERA_NEAR_Z, CAMERA_FAR_Z);

        // Combine view and projection matrix
        view_proj_matrix = XMMatrixMultiply(view_matrix, projection_matrix);
        inv_view_proj_matrix = XMMatrixInverse(nullptr, view_proj_matrix);

        // Transpose matrix for HLSL
        view_proj_matrix = XMMatrixTranspose(view_proj_matrix);
        inv_view_proj_matrix = XMMatrixTranspose(inv_view_proj_matrix);
    }

    // Prepare marble bust world matrix data
    {
        // Calculate world matrix
        XMVECTOR translation_vec = XMLoadFloat3(&marble_bust_position);
        XMVECTOR rotation_vec = XMQuaternionRotationRollPitchYaw(
            XMConvertToRadians(marble_bust_rotation.x),
            XMConvertToRadians(marble_bust_rotation.y),
            XMConvertToRadians(marble_bust_rotation.z));
        XMVECTOR scale_vec = XMLoadFloat3(&marble_bust_scale);

        XMMATRIX translation_matrix = XMMatrixTranslationFromVector(translation_vec);
        XMMATRIX rotation_matrix = XMMatrixRotationQuaternion(rotation_vec);
        XMMATRIX scale_matrix = XMMatrixScalingFromVector(scale_vec);

        XMMATRIX world_matrix = scale_matrix * rotation_matrix * translation_matrix;

        // Transpose matrix for HLSL
        marble_bust_world_buffer.world_matrix = XMMatrixTranspose(world_matrix);
        marble_bust_world_buffer.world_inverse_transpose = XMMatrixTranspose(XMMatrixInverse(nullptr, world_matrix));
    }

    // Prepare floor world matrix data
    {
        // Calculate world matrix
        XMVECTOR translation_vec = XMLoadFloat3(&floor_position);
        XMVECTOR rotation_vec = XMQuaternionRotationRollPitchYaw(
            XMConvertToRadians(floor_rotation.x),
            XMConvertToRadians(floor_rotation.y),
            XMConvertToRadians(floor_rotation.z));
        XMVECTOR scale_vec = XMLoadFloat3(&floor_scale);

        XMMATRIX translation_matrix = XMMatrixTranslationFromVector(translation_vec);
        XMMATRIX rotation_matrix = XMMatrixRotationQuaternion(rotation_vec);
        XMMATRIX scale_matrix = XMMatrixScalingFromVector(scale_vec);

        XMMATRIX world_matrix = scale_matrix * rotation_matrix * translation_matrix;

        // Transpose matrix for HLSL
        floor_world_buffer.world_matrix = XMMatrixTranspose(world_matrix);
        floor_world_buffer.world_inverse_transpose = XMMatrixTranspose(XMMatrixInverse(nullptr, world_matrix));
    }

    // Prepare clear color
    XMFLOAT4 clear_color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    // Prepare view port
    D3D12_VIEWPORT view_port = {};
    view_port.Width = static_cast<float>(CLIENT_WIDTH);
    view_port.Height = static_cast<float>(CLINT_HEIGHT);
    view_port.MinDepth = 0.0f;
    view_port.MaxDepth = 1.0f;
    view_port.TopLeftX = 0.0f;
    view_port.TopLeftY = 0.0f;

    // Prepare scissor rect
    D3D12_RECT scissor_rect = {};
    scissor_rect.top = 0;
    scissor_rect.bottom = CLINT_HEIGHT;
    scissor_rect.left = 0;
    scissor_rect.right = CLIENT_WIDTH;

    /*******************************************************************************************************************
     * Run message loop
    /******************************************************************************************************************/

    bool is_runnning = true;
    mono_graphics_service_test::RunMessageLoop(is_runnning, [&]() -> bool
    {
        bool loop_result = true;

        // Create view of the graphics service
        std::unique_ptr<mono_service::ServiceView> service_view = graphics_service_proxy->CreateView();

        // Cast to graphics service view
        mono_graphics_service::GraphicsServiceView& graphics_service_view
            = dynamic_cast<mono_graphics_service::GraphicsServiceView&>(*service_view);

        // Get current frame index
        UINT current_frame_index = graphics_service_view.GetCurrentBackBufferIndex(&swap_chain_handle);

        /***************************************************************************************************************
         * Upload light
        /**************************************************************************************************************/

        {
            // Create command list
            std::unique_ptr<mono_service::ServiceCommandList> command_list
                = graphics_service_proxy->CreateCommandList();
            mono_graphics_service::GraphicsCommandList& graphics_command_list
                = dynamic_cast<mono_graphics_service::GraphicsCommandList&>(*command_list);

            // Set swap chain handle
            graphics_command_list.SetSwapChainHandle(&swap_chain_handle);

            // Set command set handle for current frame
            graphics_command_list.SetCommandSetHandle(&command_set_handles[current_frame_index]);

            // Upload lights
            graphics_command_list.UploadLight(&directional_light_handle);
            graphics_command_list.UploadLight(&ambient_light_handle);
            graphics_command_list.UploadLight(&point_light_handle);

            // Submit command list to graphics service
            graphics_service_proxy->SubmitCommandList(std::move(command_list));
        }

        /***************************************************************************************************************
         * Update buffer
        /**************************************************************************************************************/

        {
            // Create command list
            std::unique_ptr<mono_service::ServiceCommandList> command_list
                = graphics_service_proxy->CreateCommandList();
            mono_graphics_service::GraphicsCommandList& graphics_command_list
                = dynamic_cast<mono_graphics_service::GraphicsCommandList&>(*command_list);

            // Set command set handle for current frame
            graphics_command_list.SetCommandSetHandle(&command_set_handles[current_frame_index]);

            // Update view-projection matrix
            graphics_command_list.UpdateMatrixBuffer(
                &view_proj_mat_buffer_handle, std::make_unique<XMMATRIX>(view_proj_matrix));

            // Update inverse view-projection matrix
            graphics_command_list.UpdateMatrixBuffer(
                &inv_view_proj_mat_buffer_handle, std::make_unique<XMMATRIX>(inv_view_proj_matrix));

            // Update marble bust world matrix
            graphics_command_list.UpdateWorldBufferForGeometryPass(
                &marble_bust_world_buffer_handle, 
                std::make_unique<render_graph::geometry_pass::WorldBuffer>(marble_bust_world_buffer));

            // Update floor world matrix
            graphics_command_list.UpdateWorldBufferForGeometryPass(
                &floor_world_buffer_handle, 
                std::make_unique<render_graph::geometry_pass::WorldBuffer>(floor_world_buffer));

            // Update directional light view-projection matrix
            graphics_command_list.UpdateLightViewProjMatrixBuffer(
                &directional_light_handle, 
                std::make_unique<XMFLOAT3>(DIRECTIONAL_LIGHT_POSITION), 
                std::make_unique<XMFLOAT3>(DIRECTIONAL_LIGHT_ROTATION));

            // Update ambient light view-projection matrix
            graphics_command_list.UpdateLightViewProjMatrixBuffer(
                &ambient_light_handle, 
                std::make_unique<XMFLOAT3>(AMBIENT_LIGHT_POSITION), 
                std::make_unique<XMFLOAT3>(AMBIENT_LIGHT_ROTATION));

            // Update point light view-projection matrix
            graphics_command_list.UpdateLightViewProjMatrixBuffer(
                &point_light_handle, 
                std::make_unique<XMFLOAT3>(POINT_LIGHT_POSITION), 
                std::make_unique<XMFLOAT3>(POINT_LIGHT_ROTATION));

            // Update lights buffer
            graphics_command_list.UpdateLightsBuffer(
                &light_buffer_handles[current_frame_index], &light_buffer_upload_buffer_handles[current_frame_index],
                MAX_LIGHT_COUNT);

            {
                // Create shadow composition config buffer data
                std::unique_ptr<render_graph::shadow_composition_pass::ShadowCompositionConfigBuffer> shadow_comp_config
                = std::make_unique<render_graph::shadow_composition_pass::ShadowCompositionConfigBuffer>();
                shadow_comp_config->screen_size.x = static_cast<float>(CLIENT_WIDTH);
                shadow_comp_config->screen_size.y = static_cast<float>(CLINT_HEIGHT);
                shadow_comp_config->shadow_bias = SHADOW_BIAS;
                shadow_comp_config->slope_scaled_bias = SHADOW_SLOPE_SCALED_BIAS;
                shadow_comp_config->slope_bias_exponent = SHADOW_SLOPE_BIAS_EXPONENT;

                // Update shadow composition config buffer
                graphics_command_list.UpdateShadowCompositionConfigBuffer(
                    &shadow_composition_config_buffer_handle, std::move(shadow_comp_config));
            }

            // Submit command list to graphics service
            graphics_service_proxy->SubmitCommandList(std::move(command_list));
        }

        /***************************************************************************************************************
         * Shadowing
        /**************************************************************************************************************/

        {
            // Create command list
            std::unique_ptr<mono_service::ServiceCommandList> command_list
                = graphics_service_proxy->CreateCommandList();
            mono_graphics_service::GraphicsCommandList& graphics_command_list
                = dynamic_cast<mono_graphics_service::GraphicsCommandList&>(*command_list);

            // Set swap chain handle
            graphics_command_list.SetSwapChainHandle(&swap_chain_handle);

            // Set command set handle for current frame
            graphics_command_list.SetCommandSetHandle(&command_set_handles[current_frame_index]);

            // Cast shadow for directional light
            graphics_command_list.CastShadow(&directional_light_handle);

            // Draw marble bust (because it casts shadow)
            for (size_t i = 0; i < marble_bust_vertex_buffer_handle.size(); ++i)
            {
                graphics_command_list.DrawShadowCasterMesh(
                    &marble_bust_world_buffer_handle,
                    &marble_bust_vertex_buffer_handle[i], &marble_bust_index_buffer_handle[i], marble_bust_index_count[i]);
            }

            // Draw floor (because it casts shadow)
            for (size_t i = 0; i < floor_vertex_buffer_handle.size(); ++i)
            {
                graphics_command_list.DrawShadowCasterMesh(
                    &floor_world_buffer_handle,
                    &floor_vertex_buffer_handle[i], &floor_index_buffer_handle[i], floor_index_count[i]);
            }

            // Submit command list to graphics service
            graphics_service_proxy->SubmitCommandList(std::move(command_list));
        }

        /*******************************************************************************************************************
         * Draw mesh
        /******************************************************************************************************************/

        {
            // Create command list
            std::unique_ptr<mono_service::ServiceCommandList> command_list
                = graphics_service_proxy->CreateCommandList();
            mono_graphics_service::GraphicsCommandList& graphics_command_list
                = dynamic_cast<mono_graphics_service::GraphicsCommandList&>(*command_list);

            // Set swap chain handle
            graphics_command_list.SetSwapChainHandle(&swap_chain_handle);

            // Set command set handle for current frame
            graphics_command_list.SetCommandSetHandle(&command_set_handles[current_frame_index]);

            // Draw marble bust
            for (size_t i = 0; i < marble_bust_vertex_buffer_handle.size(); ++i)
            {
                graphics_command_list.DrawMesh(
                    &marble_bust_world_buffer_handle,
                    &marble_bust_phong_material_handle,
                    &marble_bust_vertex_buffer_handle[i], &marble_bust_index_buffer_handle[i], marble_bust_index_count[i]);
            }

            // Draw floor
            for (size_t i = 0; i < floor_vertex_buffer_handle.size(); ++i)
            {
                graphics_command_list.DrawMesh(
                    &floor_world_buffer_handle,
                    &floor_lambert_material_handle,
                    &floor_vertex_buffer_handle[i], &floor_index_buffer_handle[i], floor_index_count[i]);
            }

            // Submit command list to graphics service
            graphics_service_proxy->SubmitCommandList(std::move(command_list));
        }

        /*******************************************************************************************************************
         * Add render passes
        /******************************************************************************************************************/

        {
            // Create command list
            std::unique_ptr<mono_service::ServiceCommandList> command_list
                = graphics_service_proxy->CreateCommandList();
            mono_graphics_service::GraphicsCommandList& graphics_command_list
                = dynamic_cast<mono_graphics_service::GraphicsCommandList&>(*command_list);

            // Set command set handle for current frame
            graphics_command_list.SetCommandSetHandle(&command_set_handles[current_frame_index]);

            // Set target swap chain handle
            graphics_command_list.SetSwapChainHandle(&swap_chain_handle);

            // Add light upload pass to graph
            render_graph::Light::LightConfigBuffer light_config;
            light_config.screen_size.x = static_cast<float>(CLIENT_WIDTH);
            light_config.screen_size.y = static_cast<float>(CLINT_HEIGHT);
            light_config.max_lights = MAX_LIGHT_COUNT;
            light_config.shadow_intensity = SHADOW_INTENSITY;
            light_config.camera_world_matrix = camera_world_matrix;
            light_config.lambert_material_id = render_graph::LambertMaterialTypeHandle::ID();
            light_config.phong_material_id = render_graph::PhongMaterialTypeHandle::ID();
            light_config.directional_light_id = render_graph::DirectionalLightTypeHandle::ID();
            light_config.ambient_light_id = render_graph::AmbientLightTypeHandle::ID();
            light_config.point_light_id = render_graph::PointLightTypeHandle::ID();

            // Add light upload pass to render graph
            graphics_command_list.AddLightUploadPassToGraph(
                &light_buffer_upload_buffer_handles[current_frame_index], 
                &light_config_buffer_handle, std::move(light_config));

            // Add buffer update pass to render graph
            graphics_command_list.AddBufferUploadPassToGraph();

            // Add texture upload pass to render graph
            graphics_command_list.AddTextureUploadPassToGraph();

            // Add shadowing pass to render graph
            graphics_command_list.AddShadowingPassToGraph(current_frame_index);

            // Add geometry pass to render graph
            graphics_command_list.AddGeometryPassToGraph(
                &gbuffer_texture_handles[current_frame_index], &depth_stencil_texture_handles[current_frame_index],
                &view_proj_mat_buffer_handle, view_port, scissor_rect);

            // Add Imgui pass to render graph
            graphics_command_list.AddImguiPassToGraph(
                &imgui_render_target_texture_handles[current_frame_index], 
                mono_graphics_service_test::DrawUI, &imgui_context_handle);

            // Add shadow composition pass to render graph
            graphics_command_list.AddShadowCompositionPassToGraph(
                &shadow_composition_render_target_texture_handles[current_frame_index],
                &shadow_composition_config_buffer_handle,
                &inv_view_proj_mat_buffer_handle,
                &depth_stencil_texture_handles[current_frame_index],
                &gbuffer_texture_handles[current_frame_index],
                &full_screen_triangle_vertex_buffer_handle, &full_screen_triangle_index_buffer_handle,
                view_port, scissor_rect);

            // Add lighting pass to render graph
            graphics_command_list.AddLightingPassToGraph(
                &lighting_render_target_texture_handles[current_frame_index],
                &inv_view_proj_mat_buffer_handle,
                &light_config_buffer_handle,
                &light_buffer_handles[current_frame_index],
                &depth_stencil_texture_handles[current_frame_index],
                &gbuffer_texture_handles[current_frame_index],
                &shadow_composition_render_target_texture_handles[current_frame_index],
                &full_screen_triangle_vertex_buffer_handle, &full_screen_triangle_index_buffer_handle,
                view_port, scissor_rect);

            // Add composition pass to render graph
            graphics_command_list.AddCompositionPassToGraph(
                &swap_chain_handle, clear_color,
                &lighting_render_target_texture_handles[current_frame_index][(uint32_t)render_graph::lighting_pass::RenderTargetIndex::FINAL_COLOR],
                &imgui_render_target_texture_handles[current_frame_index],
                &full_screen_triangle_vertex_buffer_handle, &full_screen_triangle_index_buffer_handle,
                view_port, scissor_rect);

            // Submit command list to graphics service
            graphics_service_proxy->SubmitCommandList(std::move(command_list));
        }

        /*******************************************************************************************************************
         * Update graphics service
        /******************************************************************************************************************/

        service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
        {
            // Get graphics service
            mono_service::Service& service = registry.Get(mono_graphics_service::GraphicsServiceHandle::ID());
            mono_graphics_service::GraphicsService& graphics_service
                = dynamic_cast<mono_graphics_service::GraphicsService&>(service);

            // Pre-update
            loop_result = graphics_service.PreUpdate();
            if (!loop_result)
                return; // Stop running

            // Update
            loop_result = graphics_service.Update();
            if (!loop_result)
                return; // Stop running

            // Post-update
            loop_result = graphics_service.PostUpdate();
            if (!loop_result)
                return; // Stop running
        });

        return loop_result;
    });

    /*******************************************************************************************************************
     * Cleanup
    /******************************************************************************************************************/

    // Cleanup service
    service_registry.reset();
    
    // Cleanup window
    mono_graphics_service_test::CleanupWindow(hwnd, wc);

    // Co-uninitialize COM library
    CoUninitialize();
}