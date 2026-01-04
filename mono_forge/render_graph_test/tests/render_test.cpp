#include "render_graph_test/pch.h"

using namespace DirectX;

#include "imgui/include/imgui.h"
#include "imgui/include/imgui_internal.h" // DockBuilder API
#include "imgui/include/imgui_impl_win32.h"
#include "imgui/include/imgui_impl_dx12.h"

#include "directx12_util/include/wrapper.h"
#include "directx12_util/include/helper.h"

#include "utility_header/file_loader.h"
#include "mono_forge_model/include/mfm.h"

#include "directxtex/DirectXTex.h"

#include "render_graph/include/render_graph.h"
#include "render_graph/include/resource_manager.h"
#include "render_graph/include/heap_manager.h"
#include "render_graph/include/command_manager.h"
#include "render_graph/include/imgui_context_manager.h"
#include "render_graph/include/render_pass_context.h"
#include "render_graph/include/material_manager.h"
#include "render_graph/include/light_manager.h"

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


// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace render_graph_test
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
constexpr wchar_t WINDOW_CLASS_NAME[] = L"Render Graph Test Window Class";
constexpr wchar_t WINDOW_TITLE[] = L"Render Graph Test";
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 800;
constexpr int WINDOW_POS_X = 100;
constexpr int WINDOW_POS_Y = 100;

// Swap chain settings
constexpr UINT SWAP_CHAIN_BUFFER_COUNT = 2;
constexpr UINT SWAP_CHAIN_SYNC_INTERVAL = 1;
constexpr DXGI_SWAP_EFFECT SWAP_CHAIN_EFFECT = DXGI_SWAP_EFFECT_FLIP_DISCARD;

// Heap sizes
constexpr UINT SRV_DESCRIPTOR_COUNT = 100;
constexpr UINT RTV_DESCRIPTOR_COUNT = 100;
constexpr UINT DSV_DESCRIPTOR_COUNT = 100;

// Object Model file path
constexpr const char* MODEL_FILE_PATH = "../resources/render_graph_test/marble_bust_01_1k/marble_bust_01_1k.mfm";
constexpr const wchar_t* MODEL_ALBEDO_TEXTURE_PATH = L"../resources/render_graph_test/marble_bust_01_1k/marble_bust_01_diff_1k.png";
constexpr const wchar_t* MODEL_NORMAL_TEXTURE_PATH = L"../resources/render_graph_test/marble_bust_01_1k/marble_bust_01_nor_dx_1k.png";
constexpr const wchar_t* MODEL_AO_TEXTURE_PATH = L"../resources/render_graph_test/marble_bust_01_1k/marble_bust_01_ao_1k.png";
constexpr const wchar_t* MODEL_ROUGHNESS_TEXTURE_PATH = L"../resources/render_graph_test/marble_bust_01_1k/marble_bust_01_rough_1k.png";

// Floor Model file path
constexpr const char* FLOOR_MODEL_FILE_PATH = "../resources/render_graph_test/floor/floor.mfm";

// Application context structure
struct AppContext
{
    /*******************************************************************************************************************
     * Window　related
    /******************************************************************************************************************/

    HWND hwnd = nullptr;
    WNDCLASSEXW wc = {};
    float main_scale = 1.0f;

    UINT client_width = WINDOW_WIDTH;
    UINT client_height = WINDOW_HEIGHT;

    /*******************************************************************************************************************
     * Render graph related
    /******************************************************************************************************************/

    std::unique_ptr<dx12_util::DXFactory> dx_factory = nullptr; // DXFactory instance
    std::unique_ptr<dx12_util::Device> dx_device = nullptr; // Device instance
    std::unique_ptr<dx12_util::CommandQueue> dx_command_queue = nullptr; // CommandQueue instance

    std::unique_ptr<render_graph::ResourceContainer> resource_container = nullptr; // Resource container
    std::unique_ptr<render_graph::ResourceManager> resource_manager = nullptr; // Resource manager singleton
    std::unique_ptr<render_graph::ResourceAdder> resource_adder = nullptr; // Resource adder
    std::unique_ptr<render_graph::ResourceEraser> resource_eraser = nullptr; // Resource eraser

    std::unique_ptr<dx12_util::DescriptorHeap> rtv_heap = nullptr; // RTV descriptor heap
    std::unique_ptr<dx12_util::DescriptorHeapAllocator> rtv_heap_allocator = nullptr; // RTV descriptor heap allocator
    std::unique_ptr<dx12_util::DescriptorHeap> srv_heap = nullptr; // SRV descriptor heap
    std::unique_ptr<dx12_util::DescriptorHeapAllocator> srv_heap_allocator = nullptr; // SRV descriptor heap allocator
    std::unique_ptr<dx12_util::DescriptorHeap> dsv_heap = nullptr; // DSV descriptor heap
    std::unique_ptr<dx12_util::DescriptorHeapAllocator> dsv_heap_allocator = nullptr; // DSV descriptor heap allocator
    std::unique_ptr<render_graph::HeapManager> heap_manager = nullptr; // Heap manager

    std::unique_ptr<render_graph::CommandSetContainer> command_set_container = nullptr; // Command set container
    std::unique_ptr<render_graph::CommandSetManager> command_set_manager = nullptr; // Command set manager
    std::unique_ptr<render_graph::CommandSetAdder> command_set_adder = nullptr; // Command set adder
    std::unique_ptr<render_graph::CommandSetEraser> command_set_eraser = nullptr; // Command set eraser

    std::unique_ptr<render_graph::ImguiContextContainer> imgui_context_container = nullptr;
    std::unique_ptr<render_graph::ImguiContextManager> imgui_context_manager = nullptr;
    std::unique_ptr<render_graph::ImguiContextAdder> imgui_context_adder = nullptr;
    std::unique_ptr<render_graph::ImguiContextEraser> imgui_context_eraser = nullptr;

    std::unique_ptr<render_graph::MaterialTypeHandleIDGenerator> material_type_handle_id_generator = nullptr; // Material type handle ID generator
    std::unique_ptr<render_graph::MaterialContainer> material_container = nullptr; // Material container
    std::unique_ptr<render_graph::MaterialManager> material_manager = nullptr; // Material manager singleton
    std::unique_ptr<render_graph::MaterialAdder> material_adder = nullptr; // Material adder
    std::unique_ptr<render_graph::MaterialEraser> material_eraser = nullptr; // Material eraser

    std::unique_ptr<render_graph::LightTypeHandleIDGenerator> light_type_handle_id_generator = nullptr; // Light type handle ID generator
    std::unique_ptr<render_graph::LightContainer> light_container = nullptr; // Light container
    std::unique_ptr<render_graph::LightManager> light_manager = nullptr; // Light manager singleton
    std::unique_ptr<render_graph::LightAdder> light_adder = nullptr; // Light adder
    std::unique_ptr<render_graph::LightEraser> light_eraser = nullptr; // Light eraser

    /*******************************************************************************************************************
     * Main loop related
    /******************************************************************************************************************/

    bool running = true; // Running flag
};

bool InitWindow(AppContext& app_context)
{
    // Make process DPI aware and obtain main monitor scale
    ImGui_ImplWin32_EnableDpiAwareness();
    app_context.main_scale 
        = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

    // Register window class
    app_context.wc = 
    { 
        sizeof(app_context.wc), CS_CLASSDC, WndProc, 0L, 0L, 
        GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, WINDOW_CLASS_NAME, nullptr 
    };
    if (!::RegisterClassExW(&app_context.wc))
        return false;

    // Create window
    app_context.hwnd = ::CreateWindowW
    (
        app_context.wc.lpszClassName, WINDOW_TITLE, WS_OVERLAPPEDWINDOW, 
        WINDOW_POS_X, WINDOW_POS_Y, 
        static_cast<int>(WINDOW_WIDTH * app_context.main_scale), 
        static_cast<int>(WINDOW_HEIGHT * app_context.main_scale), 
        nullptr, nullptr, app_context.wc.hInstance, nullptr
    );
    if (app_context.hwnd == nullptr)
    {
        ::UnregisterClassW(app_context.wc.lpszClassName, app_context.wc.hInstance);
        return false;
    }

    ::ShowWindow(app_context.hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(app_context.hwnd);

    // Get client area size
    RECT client_rect = {};
    ::GetClientRect(app_context.hwnd, &client_rect);
    app_context.client_width = client_rect.right - client_rect.left;
    app_context.client_height = client_rect.bottom - client_rect.top;

    return true;
}

void CleanupWindow(AppContext& app_context)
{
    ::DestroyWindow(app_context.hwnd);
    ::UnregisterClassW(app_context.wc.lpszClassName, app_context.wc.hInstance);
}

bool InitRenderGraph(AppContext& app_context)
{
    bool result = false;

    // Create instance of DXFactory
    app_context.dx_factory = std::make_unique<dx12_util::DXFactory>();
    result = app_context.dx_factory->Setup();
    if (!result) return false;

    // Create instance of Device
    app_context.dx_device = std::make_unique<dx12_util::Device>();
    result = app_context.dx_device->Setup(app_context.dx_factory->Get());
    if (!result) return false;

    // Create instance of CommandQueue
    app_context.dx_command_queue = std::make_unique<dx12_util::CommandQueue>();
    result = app_context.dx_command_queue->Setup(app_context.dx_device->Get());
    if (!result) return false;

    // Create resource container
    app_context.resource_container = std::make_unique<render_graph::ResourceContainer>();

    // Create singleton resource manager
    app_context.resource_manager = std::make_unique<render_graph::ResourceManager>(*app_context.resource_container);

    // Create resource adder
    app_context.resource_adder = std::make_unique<render_graph::ResourceAdder>(*app_context.resource_container);

    // Create resource eraser
    app_context.resource_eraser = std::make_unique<render_graph::ResourceEraser>(*app_context.resource_container);

    // Create RTV descriptor heap
    app_context.rtv_heap = dx12_util::DescriptorHeap::CreateInstance<dx12_util::DescriptorHeap>(
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV, RTV_DESCRIPTOR_COUNT, D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        app_context.dx_device->Get());
    if (!app_context.rtv_heap)
        return false;

    // Create RTV descriptor heap allocator
    app_context.rtv_heap_allocator 
        = dx12_util::DescriptorHeapAllocator::CreateInstance<dx12_util::DescriptorHeapAllocator>(
            *app_context.rtv_heap, app_context.dx_device->Get());
    if (!app_context.rtv_heap_allocator)
        return false;

    // Create SRV descriptor heap
    app_context.srv_heap = dx12_util::DescriptorHeap::CreateInstance<dx12_util::DescriptorHeap>(
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, SRV_DESCRIPTOR_COUNT, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
        app_context.dx_device->Get());
    if (!app_context.srv_heap)
        return false;

    // Create SRV descriptor heap allocator
    app_context.srv_heap_allocator 
        = dx12_util::DescriptorHeapAllocator::CreateInstance<dx12_util::DescriptorHeapAllocator>(
            *app_context.srv_heap, app_context.dx_device->Get());
    if (!app_context.srv_heap_allocator)
        return false;

    // Create DSV descriptor heap
    app_context.dsv_heap = dx12_util::DescriptorHeap::CreateInstance<dx12_util::DescriptorHeap>(
        D3D12_DESCRIPTOR_HEAP_TYPE_DSV, DSV_DESCRIPTOR_COUNT, D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        app_context.dx_device->Get());
    if (!app_context.dsv_heap)
        return false;

    // Create DSV descriptor heap allocator
    app_context.dsv_heap_allocator 
        = dx12_util::DescriptorHeapAllocator::CreateInstance<dx12_util::DescriptorHeapAllocator>(
            *app_context.dsv_heap, app_context.dx_device->Get());
    if (!app_context.dsv_heap_allocator)
        return false;

    // Create heap manager
    app_context.heap_manager = std::make_unique<render_graph::HeapManager>(
        *app_context.srv_heap, *app_context.srv_heap_allocator,
        *app_context.rtv_heap, *app_context.rtv_heap_allocator,
        *app_context.dsv_heap, *app_context.dsv_heap_allocator);

    // Create command set container
    app_context.command_set_container = std::make_unique<render_graph::CommandSetContainer>();

    // Create command set manager
    app_context.command_set_manager 
        = std::make_unique<render_graph::CommandSetManager>(*app_context.command_set_container);

    // Create command set adder
    app_context.command_set_adder 
        = std::make_unique<render_graph::CommandSetAdder>(*app_context.command_set_container);

    // Create command set eraser
    app_context.command_set_eraser 
        = std::make_unique<render_graph::CommandSetEraser>(*app_context.command_set_container);

    // Create ImGui context container
    app_context.imgui_context_container 
        = std::make_unique<render_graph::ImguiContextContainer>();

    // Create ImGui context manager
    app_context.imgui_context_manager 
        = std::make_unique<render_graph::ImguiContextManager>(
            *app_context.imgui_context_container);

    // Create ImGui context adder
    app_context.imgui_context_adder 
        = std::make_unique<render_graph::ImguiContextAdder>(
            *app_context.imgui_context_container);

    // Create ImGui context eraser
    app_context.imgui_context_eraser 
        = std::make_unique<render_graph::ImguiContextEraser>(
            *app_context.imgui_context_container);

    // Create material type handle ID generator
    app_context.material_type_handle_id_generator 
        = std::make_unique<render_graph::MaterialTypeHandleIDGenerator>();

    // Create material container
    app_context.material_container 
        = std::make_unique<render_graph::MaterialContainer>();

    // Create material manager singleton
    app_context.material_manager 
        = std::make_unique<render_graph::MaterialManager>(
            *app_context.material_container);

    // Create material adder
    app_context.material_adder 
        = std::make_unique<render_graph::MaterialAdder>(
            *app_context.material_container);

    // Create material eraser
    app_context.material_eraser 
        = std::make_unique<render_graph::MaterialEraser>(
            *app_context.material_container);

    // Create light type handle ID generator
    app_context.light_type_handle_id_generator 
        = std::make_unique<render_graph::LightTypeHandleIDGenerator>();

    // Create light container
    app_context.light_container 
        = std::make_unique<render_graph::LightContainer>();

    // Create light manager singleton
    app_context.light_manager 
        = std::make_unique<render_graph::LightManager>(
            *app_context.light_container);

    // Create light adder
    app_context.light_adder 
        = std::make_unique<render_graph::LightAdder>(
            *app_context.light_container);

    // Create light eraser
    app_context.light_eraser 
        = std::make_unique<render_graph::LightEraser>(
            *app_context.light_container);

    return true;
}

void CleanupRenderGraph(AppContext& app_context)
{
    app_context.material_eraser.reset();
    app_context.material_adder.reset();
    app_context.material_manager.reset();
    app_context.material_container.reset();

    app_context.imgui_context_eraser.reset();
    app_context.imgui_context_adder.reset();
    app_context.imgui_context_manager.reset();
    app_context.imgui_context_container.reset();

    app_context.command_set_eraser.reset();
    app_context.command_set_adder.reset();
    app_context.command_set_manager.reset();
    app_context.command_set_container.reset();

    app_context.resource_manager.reset();
    app_context.resource_adder.reset();
    app_context.resource_eraser.reset();
    app_context.resource_container.reset();

    app_context.heap_manager.reset();
    app_context.srv_heap.reset();
    app_context.srv_heap_allocator.reset();
    app_context.rtv_heap.reset();
    app_context.rtv_heap_allocator.reset();
    app_context.dsv_heap.reset();
    app_context.dsv_heap_allocator.reset();

    app_context.dx_command_queue.reset();
    app_context.dx_device.reset();
    app_context.dx_factory.reset();
};

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

bool InitRenderTest(AppContext& app_context)
{
    // Initialize window
    bool result = render_graph_test::InitWindow(app_context);
    if (!result)
        return false;
    
    // Initialize render graph
    result = render_graph_test::InitRenderGraph(app_context);
    if (!result)
        return false;
};

void CleanupRenderTest(AppContext& app_context)
{
    render_graph_test::CleanupRenderGraph(app_context);
    render_graph_test::CleanupWindow(app_context);
}

bool ExecuteRenderGraph(
    render_graph::RenderGraph& render_graph,
    render_graph::ResourceHandle* target_swap_chain_handle,
    render_graph::CommandSetHandle* target_command_handle)
{
    // Compile render graph
    bool result = render_graph.Compile();
    if (!result)
        return false; // Stop on failure

    // Get command set
    render_graph::CommandSetManager& command_set_manager = render_graph::CommandSetManager::GetInstance();
    render_graph::CommandSet* command_set = nullptr;
    command_set_manager.WithLock([&](render_graph::CommandSetManager& manager)
    {
        command_set = &manager.GetCommand(target_command_handle);
    });

    // Reset command
    command_set->ResetCommand();

    // Create render pass context
    std::unique_ptr<render_graph::RenderPassContext> context 
        = std::make_unique<render_graph::RenderPassContext>(command_set->GetCommandList());

    // Execute render graph
    result = render_graph.Execute(*context);
    if (!result)
        return false; // Stop on failure

    // Close command
    command_set->CloseCommand();

    // Get dx command queue
    dx12_util::CommandQueue& command_queue = dx12_util::CommandQueue::GetInstance();

    // Execute command list
    ID3D12CommandList* dx_command_list = command_set->GetCommandList().Get();
    command_queue.Get()->ExecuteCommandLists(1, &dx_command_list);

    render_graph::ResourceManager& resource_manager = render_graph::ResourceManager::GetInstance();
    resource_manager.WithLock([&](render_graph::ResourceManager& manager)
    {
        // Create write access token for swap chain
        render_graph::ResourceAccessToken write_token;
        write_token.PermitAccess(target_swap_chain_handle);

        // Get swap chain resource for writing
        dx12_util::Resource& resource 
            = manager.GetWriteResource(target_swap_chain_handle, write_token);

        // Cast to swap chain
        dx12_util::SwapChain& swap_chain 
            = dynamic_cast<dx12_util::SwapChain&>(resource);

        // Present swap chain
        result = swap_chain.Present();
        assert(result);

        // Wait for previous frame
        result = swap_chain.WaitForPreviousFrame(dx12_util::CommandQueue::GetInstance().Get());
        assert(result);
    });

    // Clear render graph for next frame
    render_graph.Clear();

    return true; // Successfully executed render graph
}

class ElementSetting
{
public:
    virtual ~ElementSetting() = default;
};

class ElementEditor
{
public:
    virtual ~ElementEditor() = default;

    // Edit element settings via UI
    virtual void Edit(ElementSetting& settings) = 0;
};

class CameraSetting : public ElementSetting
{
public:
    virtual ~CameraSetting() override = default;

    std::string name = "";
    bool active_self = true;

    XMFLOAT3 translation = XMFLOAT3(0.0f, 0.0f, 0.0f);
    XMFLOAT3 rotation = XMFLOAT3(0.0f, 0.0f, 0.0f); // In degrees
    float fov_y = 60.0f; // In degrees
    float near_z = 0.1f;
    float far_z = 1000.0f;
    float aspect_ratio = 16.0f / 9.0f;

    XMMATRIX view_proj_matrix = XMMatrixIdentity();
    XMMATRIX inv_view_proj_matrix = XMMatrixIdentity();
    XMMATRIX world_matrix = XMMatrixIdentity();

    render_graph::ResourceHandle view_proj_matrix_buffer_handle = render_graph::ResourceHandle();
    render_graph::ResourceHandle inv_view_proj_matrix_buffer_handle = render_graph::ResourceHandle();
};

class CameraUIEditor : public ElementEditor
{
public:
    CameraUIEditor() = default;
    ~CameraUIEditor() = default;

    void Edit(ElementSetting& settings) override
    {
        // Cast to CameraSetting
        CameraSetting* camera_setting = dynamic_cast<CameraSetting*>(&settings);
        assert(camera_setting != nullptr && "Failed to cast to CameraSetting");

        ImGui::Text(camera_setting->name.c_str());
        ImGui::DragFloat3(("Translation##" + camera_setting->name).c_str(), &camera_setting->translation.x, 0.1f);
        ImGui::DragFloat3(("Rotation##" + camera_setting->name).c_str(), &camera_setting->rotation.x, 1.0f);
        ImGui::DragFloat(("FOV Y##" + camera_setting->name).c_str(), &camera_setting->fov_y, 1.0f, 1.0f, 179.0f);
        ImGui::DragFloat(("Near Z##" + camera_setting->name).c_str(), &camera_setting->near_z, 0.01f, 0.01f, camera_setting->far_z - 0.01f);
        ImGui::DragFloat(("Far Z##" + camera_setting->name).c_str(), &camera_setting->far_z, 1.0f, camera_setting->near_z + 0.01f, 10000.0f);

        XMVECTOR up_vec = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        XMVECTOR forward_vec = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
        XMVECTOR rot_vec = XMQuaternionRotationRollPitchYaw(
            XMConvertToRadians(camera_setting->rotation.x),
            XMConvertToRadians(camera_setting->rotation.y),
            XMConvertToRadians(camera_setting->rotation.z));
        XMVECTOR rotated_forward_vec = XMVector3Rotate(forward_vec, rot_vec);
        XMVECTOR rotated_up_vec = XMVector3Rotate(up_vec, rot_vec);
        XMVECTOR position_vec = XMLoadFloat3(&camera_setting->translation);

        XMMATRIX view_matrix = XMMatrixLookAtLH(
            position_vec, XMVectorAdd(position_vec, rotated_forward_vec), rotated_up_vec);

        // Calculate projection matrix
        XMMATRIX projection_matrix = XMMatrixPerspectiveFovLH(
            XMConvertToRadians(camera_setting->fov_y), camera_setting->aspect_ratio,
            camera_setting->near_z, camera_setting->far_z);

        // Combine view and projection matrices
        XMMATRIX view_proj_matrix = XMMatrixMultiply(view_matrix, projection_matrix);

        // Calculate inverse view-projection matrix
        XMMATRIX inv_view_proj_matrix = XMMatrixInverse(nullptr, view_proj_matrix);

        // Transpose matrix for HLSL
        camera_setting->view_proj_matrix = XMMatrixTranspose(view_proj_matrix);
        camera_setting->inv_view_proj_matrix = XMMatrixTranspose(inv_view_proj_matrix);

        // Update camera world matrix
        XMMATRIX translation_matrix = XMMatrixTranslationFromVector(position_vec);
        XMMATRIX rotation_matrix = XMMatrixRotationQuaternion(rot_vec);
        XMMATRIX camera_world_matrix = XMMatrixMultiply(rotation_matrix, translation_matrix);

        // Transpose camera world matrix for HLSL
        camera_setting->world_matrix = XMMatrixTranspose(camera_world_matrix);
    }
};

// Camera context structure
struct CameraContext
{
    std::unique_ptr<ElementSetting> camera_settings = nullptr;
    std::unique_ptr<ElementEditor> camera_editor = nullptr;
};
static std::unordered_map<uint32_t, CameraContext> g_camera_contexts;

enum class CameraID : uint32_t
{
    MAIN_CAMERA,
    COUNT,
};

class MaterialSetting : public ElementSetting
{
public:
    virtual ~MaterialSetting() override = default;

    std::string name = "";
    XMFLOAT4 base_color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    render_graph::MaterialHandle material_handle = render_graph::MaterialHandle();
    render_graph::MaterialTypeHandleID material_type_handle_id = render_graph::MaterialTypeHandleID();
};

// Lambert material editor
class LambertMaterialEditor : public ElementEditor
{
public:
    LambertMaterialEditor() = default;
    ~LambertMaterialEditor() = default;

    class Settings : public MaterialSetting
    {
    public:
        ~Settings() override = default;

        bool use_albedo_texture = false;
        bool use_normal_texture = false;
        bool use_ao_texture = false;
        bool use_emissive_texture = false;
        XMFLOAT4 emissive_color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    };

    void Edit(ElementSetting& settings) override
    {
        // Cast to Lambert material setting
        LambertMaterialEditor::Settings* lambert_setting 
            = dynamic_cast<LambertMaterialEditor::Settings*>(&settings);
        assert(lambert_setting != nullptr && "Failed to cast to LambertMaterialEditor::Settings");

        ImGui::Text(lambert_setting->name.c_str());
        ImGui::ColorEdit4(("Base Color##" + lambert_setting->name).c_str(), &lambert_setting->base_color.x);
        ImGui::Checkbox(("Use Albedo Texture##" + lambert_setting->name).c_str(), &lambert_setting->use_albedo_texture);
        ImGui::Checkbox(("Use Normal Texture##" + lambert_setting->name).c_str(), &lambert_setting->use_normal_texture);
        ImGui::Checkbox(("Use AO Texture##" + lambert_setting->name).c_str(), &lambert_setting->use_ao_texture);
        ImGui::Checkbox(("Use Emissive Texture##" + lambert_setting->name).c_str(), &lambert_setting->use_emissive_texture);
        ImGui::ColorEdit4(("Emissive Color##" + lambert_setting->name).c_str(), &lambert_setting->emissive_color.x);
    
        render_graph::MaterialManager::GetInstance().WithLock([&](render_graph::MaterialManager& manager)
        {
            // Get material
            render_graph::Material& material = manager.GetMaterial(&lambert_setting->material_handle);
            render_graph::LambertMaterial* lambert_material 
                = dynamic_cast<render_graph::LambertMaterial*>(&material);
            assert(lambert_material != nullptr && "Failed to cast to LambertMaterial");

            // Update material properties
            lambert_material->SetBaseColor(lambert_setting->base_color);
            lambert_material->SetAlbedoSource(
                lambert_setting->use_albedo_texture 
                ? render_graph::LambertMaterial::ALBEDO_SOURCE_TEXTURE 
                : render_graph::LambertMaterial::ALBEDO_SOURCE_BASE_COLOR);
            lambert_material->SetNormalSource(
                lambert_setting->use_normal_texture
                ? render_graph::LambertMaterial::NORMAL_SOURCE_TEXTURE
                : render_graph::LambertMaterial::NORMAL_SOURCE_VERTEX);
            lambert_material->SetAOSource(
                lambert_setting->use_ao_texture
                ? render_graph::LambertMaterial::AO_SOURCE_TEXTURE
                : render_graph::LambertMaterial::AO_SOURCE_NONE);
            lambert_material->SetEmissionSource(
                lambert_setting->use_emissive_texture
                ? render_graph::LambertMaterial::EMISSION_SOURCE_TEXTURE
                : render_graph::LambertMaterial::EMISSION_SOURCE_NONE);
            lambert_material->SetEmissionColor(lambert_setting->emissive_color);
        });
    }
};

// Phong material editor
class PhongMaterialEditor : public ElementEditor
{
public:
    PhongMaterialEditor() = default;
    ~PhongMaterialEditor() = default;

    class Settings : public MaterialSetting
    {
    public:
        ~Settings() override = default;

        bool use_albedo_texture = false;
        bool use_normal_texture = false;
        bool use_ao_texture = false;
        bool use_specular_texture = false;
        bool use_roughness_texture = false;
        float roughness = 0.5f;
        bool use_metallic_texture = false;
        float metallic = 0.0f;
        bool use_emissive_texture = false;
        XMFLOAT4 emissive_color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    };

    void Edit(ElementSetting& settings) override
    {
        // Cast to Phong material setting
        PhongMaterialEditor::Settings* phong_setting
            = dynamic_cast<PhongMaterialEditor::Settings*>(&settings);
        assert(phong_setting != nullptr && "Failed to cast to PhongMaterialEditor::Settings");

        ImGui::Text(phong_setting->name.c_str());
        ImGui::ColorEdit4(("Base Color##" + phong_setting->name).c_str(), &phong_setting->base_color.x);
        ImGui::Checkbox(("Use Albedo Texture##" + phong_setting->name).c_str(), &phong_setting->use_albedo_texture);
        ImGui::Checkbox(("Use Normal Texture##" + phong_setting->name).c_str(), &phong_setting->use_normal_texture);
        ImGui::Checkbox(("Use AO Texture##" + phong_setting->name).c_str(), &phong_setting->use_ao_texture);
        ImGui::Checkbox(("Use Specular Texture##" + phong_setting->name).c_str(), &phong_setting->use_specular_texture);
        ImGui::Checkbox(("Use Roughness Texture##" + phong_setting->name).c_str(), &phong_setting->use_roughness_texture);
        ImGui::DragFloat(("Roughness##" + phong_setting->name).c_str(), &phong_setting->roughness, 0.01f, 0.0f, 1.0f);
        ImGui::Checkbox(("Use Metallic Texture##" + phong_setting->name).c_str(), &phong_setting->use_metallic_texture);
        ImGui::DragFloat(("Metallic##" + phong_setting->name).c_str(), &phong_setting->metallic, 0.01f, 0.0f, 1.0f);
        ImGui::Checkbox(("Use Emissive Texture##" + phong_setting->name).c_str(), &phong_setting->use_emissive_texture);
        ImGui::ColorEdit4(("Emissive Color##" + phong_setting->name).c_str(), &phong_setting->emissive_color.x);

        render_graph::MaterialManager::GetInstance().WithLock([&](render_graph::MaterialManager& manager)
        {
            // Get material
            render_graph::Material& material = manager.GetMaterial(&phong_setting->material_handle);
            render_graph::PhongMaterial* phong_material
                = dynamic_cast<render_graph::PhongMaterial*>(&material);
            assert(phong_material != nullptr && "Failed to cast to PhongMaterial");

            // Update material properties
            phong_material->SetBaseColor(phong_setting->base_color);
            phong_material->SetAlbedoSource(
                phong_setting->use_albedo_texture
                ? render_graph::PhongMaterial::ALBEDO_SOURCE_TEXTURE
                : render_graph::PhongMaterial::ALBEDO_SOURCE_BASE_COLOR);
            phong_material->SetNormalSource(
                phong_setting->use_normal_texture
                ? render_graph::PhongMaterial::NORMAL_SOURCE_TEXTURE
                : render_graph::PhongMaterial::NORMAL_SOURCE_VERTEX);
            phong_material->SetAOSource(
                phong_setting->use_ao_texture
                ? render_graph::PhongMaterial::AO_SOURCE_TEXTURE
                : render_graph::PhongMaterial::AO_SOURCE_NONE);
            phong_material->SetSpecularSource(
                phong_setting->use_specular_texture
                ? render_graph::PhongMaterial::SPECULAR_SOURCE_TEXTURE
                : render_graph::PhongMaterial::SPECULAR_SOURCE_NONE);
            phong_material->SetRoughnessSource(
                phong_setting->use_roughness_texture
                ? render_graph::PhongMaterial::ROUGHNESS_SOURCE_TEXTURE
                : render_graph::PhongMaterial::ROUGHNESS_SOURCE_VALUE);
            phong_material->SetRoughnessValue(phong_setting->roughness);
            phong_material->SetMetalnessSource(
                phong_setting->use_metallic_texture
                ? render_graph::PhongMaterial::METALNESS_SOURCE_TEXTURE
                : render_graph::PhongMaterial::METALNESS_SOURCE_VALUE);
            phong_material->SetMetalnessValue(phong_setting->metallic);
            phong_material->SetEmissionSource(
                phong_setting->use_emissive_texture
                ? render_graph::PhongMaterial::EMISSION_SOURCE_TEXTURE
                : render_graph::PhongMaterial::EMISSION_SOURCE_COLOR);
            phong_material->SetEmissionColor(phong_setting->emissive_color);
        });
    }
};

// Material context structure
struct MaterialContext
{
    std::unique_ptr<ElementEditor> material_editor = nullptr;
    std::unique_ptr<ElementSetting> material_settings = nullptr;
};
static std::unordered_map<uint32_t, MaterialContext> g_material_contexts;
static std::unordered_map<const char*, uint32_t> g_material_name_to_id_map;
static std::vector<const char*> g_material_names;

enum class MaterialID : uint32_t
{
    MARBLE_BUST_LAMBERT_MATERIAL,
    MARBLE_BUST_PHONG_MATERIAL,
    FLOOR_LAMBERT_MATERIAL,
    COUNT,
};

class LightSetting : public ElementSetting
{
public:
    virtual ~LightSetting() override = default;
    std::string name = "";
    bool active_self = true;

    XMFLOAT4 color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    float intensity = 1.0f;

    XMFLOAT3 translation = XMFLOAT3(0.0f, 0.0f, 0.0f);

    render_graph::LightHandle light_handle = render_graph::LightHandle();
    render_graph::LightTypeHandleID light_type_handle_id = render_graph::LightTypeHandleID();
};

constexpr uint32_t DIRECTIONAL_LIGHT_SHADOW_MAP_SIZE = 2048;
constexpr uint32_t LIGHT_MAX_COUNT = 16;

// Directional light editor
class DirectionalLightEditor : public ElementEditor
{
public:
    DirectionalLightEditor() = default;
    ~DirectionalLightEditor() = default;

    class Settings : public LightSetting
    {
    public:
        ~Settings() override = default;

        render_graph::ResourceHandles shadow_map_handles;

        XMFLOAT3 rotation = XMFLOAT3(60.0f, 90.0f, 0.0f);
        float distance = 10.0f;
        float orthographic_width = 10.0f;
        float orthographic_height = 10.0f;
        float near_z = 0.1f;
        float far_z = 100.0f;
    };

    void Edit(ElementSetting& settings) override
    {
        // Cast to DirectionalLightSetting
        DirectionalLightEditor::Settings* dir_light_setting
            = dynamic_cast<DirectionalLightEditor::Settings*>(&settings);
        assert(dir_light_setting != nullptr && "Failed to cast to DirectionalLightEditor::Settings");

        ImGui::Text(dir_light_setting->name.c_str());
        ImGui::Checkbox(("Active##" + dir_light_setting->name).c_str(), &dir_light_setting->active_self);
        ImGui::ColorEdit4(("Color##" + dir_light_setting->name).c_str(), &dir_light_setting->color.x);
        ImGui::DragFloat(("Intensity##" + dir_light_setting->name).c_str(), &dir_light_setting->intensity, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat3(("Rotation##" + dir_light_setting->name).c_str(), &dir_light_setting->rotation.x, 1.0f);
        ImGui::DragFloat(("Distance##" + dir_light_setting->name).c_str(), &dir_light_setting->distance, 0.1f, 0.1f, 1000.0f);
        ImGui::DragFloat(("Ortho Width##" + dir_light_setting->name).c_str(), &dir_light_setting->orthographic_width, 0.1f, 0.1f, 1000.0f);
        ImGui::DragFloat(("Ortho Height##" + dir_light_setting->name).c_str(), &dir_light_setting->orthographic_height, 0.1f, 0.1f, 1000.0f);
        ImGui::DragFloat(("Near Z##" + dir_light_setting->name).c_str(), &dir_light_setting->near_z, 0.1f, 0.1f, dir_light_setting->far_z - 0.1f);
        ImGui::DragFloat(("Far Z##" + dir_light_setting->name).c_str(), &dir_light_setting->far_z, 1.0f, dir_light_setting->near_z + 0.1f, 10000.0f);

        render_graph::LightManager::GetInstance().WithLock([&](render_graph::LightManager& manager)
        {
            // Get light
            render_graph::Light& light = manager.GetLight(&dir_light_setting->light_handle);
            render_graph::DirectionalLight* dir_light
                = dynamic_cast<render_graph::DirectionalLight*>(&light);
            assert(dir_light != nullptr && "Failed to cast to DirectionalLight");

            // Update light properties
            dir_light->SetColor(dir_light_setting->color);
            dir_light->SetIntensity(dir_light_setting->intensity);
            dir_light->SetRotation(dir_light_setting->rotation);
            dir_light->SetDistance(dir_light_setting->distance);
            dir_light->SetOrthographicWidth(dir_light_setting->orthographic_width);
            dir_light->SetOrthographicHeight(dir_light_setting->orthographic_height);
            dir_light->SetNearZ(dir_light_setting->near_z);
            dir_light->SetFarZ(dir_light_setting->far_z);

            // Update matrices
            light.UpdateWorldMatrices();
            light.UpdateViewProjMatrix();
        });
    }
};

// Ambient light editor
class AmbientLightEditor : public ElementEditor
{
public:
    AmbientLightEditor() = default;
    ~AmbientLightEditor() = default;

    void Edit(ElementSetting& settings) override
    {
        // Cast to LightSetting
        LightSetting* ambient_light_setting = dynamic_cast<LightSetting*>(&settings);
        assert(ambient_light_setting != nullptr && "Failed to cast to LightSetting");

        ImGui::Text(ambient_light_setting->name.c_str());
        ImGui::Checkbox(("Active##" + ambient_light_setting->name).c_str(), &ambient_light_setting->active_self);
        ImGui::ColorEdit4(("Color##" + ambient_light_setting->name).c_str(), &ambient_light_setting->color.x);
        ImGui::DragFloat(("Intensity##" + ambient_light_setting->name).c_str(), &ambient_light_setting->intensity, 0.1f, 0.0f, 100.0f);

        render_graph::LightManager::GetInstance().WithLock([&](render_graph::LightManager& manager)
        {
            // Get light
            render_graph::Light& light = manager.GetLight(&ambient_light_setting->light_handle);
            render_graph::AmbientLight* ambient_light
                = dynamic_cast<render_graph::AmbientLight*>(&light);
            assert(ambient_light != nullptr && "Failed to cast to AmbientLight");

            // Update light properties
            ambient_light->SetColor(ambient_light_setting->color);
            ambient_light->SetIntensity(ambient_light_setting->intensity);

            // Update matrices
            light.UpdateWorldMatrices();
            light.UpdateViewProjMatrix();
        });
    }
};

// Point light editor
class PointLightEditor : public ElementEditor
{
public:
    PointLightEditor() = default;
    ~PointLightEditor() = default;

    class Settings : public LightSetting
    {
    public:
        ~Settings() override = default;

        XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f);
        float range = 10.0f;
    };

    void Edit(ElementSetting& settings) override
    {
        // Cast to PointLightSetting
        PointLightEditor::Settings* point_light_setting
            = dynamic_cast<PointLightEditor::Settings*>(&settings);
        assert(point_light_setting != nullptr && "Failed to cast to PointLightEditor::Settings");

        ImGui::Text(point_light_setting->name.c_str());
        ImGui::Checkbox(("Active##" + point_light_setting->name).c_str(), &point_light_setting->active_self);
        ImGui::ColorEdit4(("Color##" + point_light_setting->name).c_str(), &point_light_setting->color.x);
        ImGui::DragFloat(("Intensity##" + point_light_setting->name).c_str(), &point_light_setting->intensity, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat3(("Position##" + point_light_setting->name).c_str(), &point_light_setting->translation.x, 0.1f);
        ImGui::DragFloat(("Range##" + point_light_setting->name).c_str(), &point_light_setting->range, 0.1f, 0.1f, 1000.0f);

        render_graph::LightManager::GetInstance().WithLock([&](render_graph::LightManager& manager)
        {
            // Get light
            render_graph::Light& light = manager.GetLight(&point_light_setting->light_handle);
            render_graph::PointLight* point_light
                = dynamic_cast<render_graph::PointLight*>(&light);
            assert(point_light != nullptr && "Failed to cast to PointLight");

            // Update light properties
            point_light->SetColor(point_light_setting->color);
            point_light->SetIntensity(point_light_setting->intensity);
            point_light->SetPosition(point_light_setting->translation);
            point_light->SetRange(point_light_setting->range);

            // Update matrices
            light.UpdateWorldMatrices();
            light.UpdateViewProjMatrix();
        });
    }
};

// Light context structure
struct LightContext
{
    std::unique_ptr<ElementEditor> light_editor = nullptr;
    std::unique_ptr<ElementSetting> light_settings = nullptr;
};
static std::unordered_map<uint32_t, LightContext> g_light_contexts;

enum class LightID : uint32_t
{
    DIRECTIONAL_LIGHT,
    AMBIENT_LIGHT,
    POINT_LIGHT,
    COUNT,
};

// Simple mesh struct
struct Mesh
{
    render_graph::ResourceHandle vertex_buffer_handle = render_graph::ResourceHandle();
    render_graph::ResourceHandle index_buffer_handle = render_graph::ResourceHandle();
    size_t index_count = 0;
    render_graph::MaterialHandle material_handle = render_graph::MaterialHandle();
};

class ObjectSetting : public ElementSetting
{
public:
    virtual ~ObjectSetting() override = default;

    std::string name = "";
    bool active_self = true;

    XMFLOAT3 translation = XMFLOAT3(0.0f, 0.0f, 0.0f);
    XMFLOAT3 rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
    XMFLOAT3 scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
    uint32_t material_id = 0;

    std::vector<Mesh> meshes;
    render_graph::ResourceHandle world_buffer_handle = render_graph::ResourceHandle();
    render_graph::geometry_pass::WorldBuffer world_buffer = {};
};

// Object editor
class ObjectEditor : public ElementEditor
{
public:
    ObjectEditor() = default;
    ~ObjectEditor() = default;

    void Edit(ElementSetting& settings) override
    {
        // Cast to ObjectSetting
        ObjectSetting* object_setting = dynamic_cast<ObjectSetting*>(&settings);
        assert(object_setting != nullptr && "Failed to cast to ObjectSetting");

        ImGui::Text(object_setting->name.c_str());
        ImGui::DragFloat3(("Translation##" + object_setting->name).c_str(), &object_setting->translation.x, 0.1f);
        ImGui::DragFloat3(("Rotation##" + object_setting->name).c_str(), &object_setting->rotation.x, 1.0f);
        ImGui::DragFloat3(("Scale##" + object_setting->name).c_str(), &object_setting->scale.x, 0.1f, 0.01f);
        ImGui::ListBox(("Material##" + object_setting->name).c_str(), 
            reinterpret_cast<int*>(&object_setting->material_id), 
            g_material_names.data(), static_cast<int>(g_material_names.size()));

        XMVECTOR translation_vec = XMLoadFloat3(&object_setting->translation);
        XMVECTOR rotation_vec = XMQuaternionRotationRollPitchYaw(
            XMConvertToRadians(object_setting->rotation.x),
            XMConvertToRadians(object_setting->rotation.y),
            XMConvertToRadians(object_setting->rotation.z));
        XMVECTOR scale_vec = XMLoadFloat3(&object_setting->scale);
            
        XMMATRIX translation_matrix = XMMatrixTranslationFromVector(translation_vec);
        XMMATRIX rotation_matrix = XMMatrixRotationQuaternion(rotation_vec);
        XMMATRIX scale_matrix = XMMatrixScalingFromVector(scale_vec);

        object_setting->world_buffer.world_matrix = scale_matrix * rotation_matrix * translation_matrix;
        object_setting->world_buffer.world_inverse_transpose 
            = XMMatrixTranspose(XMMatrixInverse(nullptr, object_setting->world_buffer.world_matrix));
        
        // Transpose world matrix for HLSL
        object_setting->world_buffer.world_matrix = XMMatrixTranspose(object_setting->world_buffer.world_matrix);
        object_setting->world_buffer.world_inverse_transpose = XMMatrixTranspose(object_setting->world_buffer.world_inverse_transpose);

        // Get material context
        render_graph_test::MaterialContext& material_context  = g_material_contexts[object_setting->material_id];

        // Get material setting
        MaterialSetting* material_setting 
            = dynamic_cast<MaterialSetting*>(material_context.material_settings.get());
        assert(material_setting != nullptr && "Failed to cast to MaterialSetting");

        // Update mesh material handles
        for (auto& mesh : object_setting->meshes)
            mesh.material_handle = material_setting->material_handle;
    }
};

// Object context structure
struct ObjectContext
{
    std::unique_ptr<ElementEditor> object_editor = nullptr;
    std::unique_ptr<ElementSetting> object_settings = nullptr;
};
static std::unordered_map<uint32_t, ObjectContext> g_object_contexts;

enum class ObjectID : uint32_t
{
    MARBLE_BUST_OBJECT,
    FLOOR_OBJECT,
    COUNT,
};

// Shadow bias
static float g_shadow_bias = 0.005f;
static float g_shadow_slope_scaled_bias = 0.001f;
static float g_shadow_slope_bias_exponent = 100.0f;
static float g_shadow_intensity = 0.2f;

bool DrawUI(ImGuiContext* context)
{
    // Set current ImGui context
    ImGui::SetCurrentContext(context);

    ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    if (ImGui::TreeNode("Camera Settings"))
    {
        // Draw camera UIs
        for (auto& [id, camera_context] : g_camera_contexts)
        {
            camera_context.camera_editor->Edit(*camera_context.camera_settings);
            ImGui::Spacing();
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Material Settings"))
    {
        // Draw material UIs
        for (auto& [id, material_context] : g_material_contexts)
        {
            material_context.material_editor->Edit(*material_context.material_settings);
            ImGui::Spacing();
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Light Settings"))
    {
        // Draw light UIs
        for (auto& [id, light_context] : g_light_contexts)
        {
            light_context.light_editor->Edit(*light_context.light_settings);
            ImGui::Spacing();
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Object Settings"))
    {
        // Draw object UIs
        for (auto& [id, object_context] : g_object_contexts)
        {
            object_context.object_editor->Edit(*object_context.object_settings);
            ImGui::Spacing();
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Render Settings"))
    {
        ImGui::DragFloat("Shadow Bias", &g_shadow_bias, 0.0001f, 0.0f, 0.1f);
        ImGui::DragFloat("Shadow Slope Scaled Bias", &g_shadow_slope_scaled_bias, 0.001f, 0.0f, 0.5f);
        ImGui::DragFloat("Shadow Intensity", &g_shadow_intensity, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("Shadow Slope Exponent", &g_shadow_slope_bias_exponent, 0.1f, 1.0f, 10.0f);
        ImGui::TreePop();
    }

    ImGui::End();

    return true; // Continue running
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

bool CreateSwapChain(
    AppContext& app_context, render_graph::ResourceHandle* out_swap_chain_handle)
{
    // Create swap chain resource
    std::unique_ptr<dx12_util::SwapChain> swap_chain
        = dx12_util::SwapChain::CreateInstance<dx12_util::SwapChain>(
            render_graph_test::SWAP_CHAIN_BUFFER_COUNT, render_graph_test::SWAP_CHAIN_SYNC_INTERVAL, 
            render_graph::composition_pass::SWAP_CHAIN_EFFECT, render_graph::composition_pass::SWAP_CHAIN_FORMAT,
            L"Swap Chain",
        dx12_util::Device::GetInstance().Get(), dx12_util::DXFactory::GetInstance().Get(),
        dx12_util::CommandQueue::GetInstance().Get(), 
        app_context.hwnd, app_context.client_width, app_context.client_height);
    if (!swap_chain)
        return false; // Failure

    // Add swap chain to resource container
    *out_swap_chain_handle = app_context.resource_adder->AddResource(std::move(swap_chain));
    if (!out_swap_chain_handle->IsValid())
        return false; // Failure

    return true; // Success
}

bool CreateCommandSets(
    AppContext& app_context, render_graph::CommandSetHandles* out_command_set_handles)
{
    // Create command sets
    for (UINT i = 0; i < render_graph_test::SWAP_CHAIN_BUFFER_COUNT; ++i)
    {
        render_graph::CommandSetHandle command_set_handle = render_graph::CommandSetHandle();
        
        // Create command set
        std::unique_ptr<render_graph::CommandSet> command_set
            = render_graph::CommandSet::CreateInstance<render_graph::CommandSet>(D3D12_COMMAND_LIST_TYPE_DIRECT);
        if (!command_set)
            return false; // Failure

        // Add command set to command set container
        command_set_handle = app_context.command_set_adder->AddCommandSet(std::move(command_set));
        if (!command_set_handle.IsValid())
            return false; // Failure

        // Store command set handle
        out_command_set_handles->emplace_back(std::move(command_set_handle));
    }

    return true; // Success
}

bool CreateBuffer(
    AppContext& app_context,
    render_graph::ResourceHandle* out_buffer_handle, size_t buffer_size, const void* initial_data, 
    std::wstring debug_name = L"Unnamed Buffer")
{
    std::unique_ptr<dx12_util::Buffer> buffer
        = dx12_util::Buffer::CreateInstance<dx12_util::Buffer>(
            buffer_size, D3D12_HEAP_TYPE_UPLOAD, debug_name,
            dx12_util::Device::GetInstance().Get(), nullptr);
    if (!buffer)
        return false; // Failure

    // Upload initial data if provided
    if (initial_data)
        buffer->UpdateData(initial_data, buffer_size);

    // Add buffer to resource container
    *out_buffer_handle = app_context.resource_adder->AddResource(std::move(buffer));
    if (!out_buffer_handle->IsValid())
        return false; // Failure

    return true; // Success
}

bool CreateStructuredBuffer(
    AppContext& app_context,
    render_graph::ResourceHandle* out_buffer_handle, 
    size_t element_size, size_t element_count, dx12_util::DescriptorHeapAllocator* srv_heap_allocator,
    std::wstring debug_name = L"Unnamed Structured Buffer")
{
    std::unique_ptr<dx12_util::StructuredBuffer> buffer
        = dx12_util::StructuredBuffer::CreateInstance<dx12_util::StructuredBuffer>(
            element_size, element_count, D3D12_HEAP_TYPE_DEFAULT, false, debug_name,
            dx12_util::Device::GetInstance().Get(), srv_heap_allocator);
    if (!buffer)
        return false; // Failure

    // Add structured buffer to resource container
    *out_buffer_handle = app_context.resource_adder->AddResource(std::move(buffer));
    if (!out_buffer_handle->IsValid())
        return false; // Failure

    return true; // Success
}

bool CreateTexture2D(
    AppContext& app_context,
    render_graph::ResourceHandle* out_texture_handle,
    UINT width, UINT height, DXGI_FORMAT format,
    D3D12_HEAP_TYPE heap_type, D3D12_RESOURCE_FLAGS resource_flags, 
    D3D12_RESOURCE_STATES initial_state, D3D12_CLEAR_VALUE* clear_value, 
    const DXGI_FORMAT* srv_format, const DXGI_FORMAT* uav_format, 
    const DXGI_FORMAT* rtv_format, const DXGI_FORMAT* dsv_format,
    dx12_util::DescriptorHeapAllocator* srv_heap_allocator,
    dx12_util::DescriptorHeapAllocator* rtv_heap_allocator,
    dx12_util::DescriptorHeapAllocator* dsv_heap_allocator,
    std::wstring debug_name = L"Unnamed Texture2D")
{
    std::unique_ptr<dx12_util::Texture2D> texture
        = dx12_util::Texture2D::CreateInstance<dx12_util::Texture2D>(
            width, height, format, heap_type, resource_flags, initial_state, debug_name,
            dx12_util::Device::GetInstance().Get(), clear_value,
            srv_format, uav_format, rtv_format, dsv_format,
            srv_heap_allocator, rtv_heap_allocator, dsv_heap_allocator);
    if (!texture)
        return false; // Failure

    // Add texture to resource container
    *out_texture_handle = app_context.resource_adder->AddResource(std::move(texture));
    if (!out_texture_handle->IsValid())
        return false; // Failure

    return true; // Success
}

bool CreateImguiContext(
    AppContext& app_context,
    render_graph::ResourceHandle* out_imgui_context_handle,
    render_graph::ImguiContext::ContextCreateFunc create_func, 
    render_graph::ImguiContext::ContextDestroyFunc destroy_func)
{
    // Create a new ImguiContext instance
    std::unique_ptr<render_graph::ImguiContext> context 
        = render_graph::ImguiContext::CreateInstance<render_graph::ImguiContext>(
            app_context.hwnd, std::move(create_func), std::move(destroy_func));

    // Add ImGui context to ImGui context container
    *out_imgui_context_handle 
        = app_context.imgui_context_adder->Add(std::move(context));
    if (!out_imgui_context_handle->IsValid())
        return false; // Failure

    return true; // Success
}

UINT GetCurrentFrameIndex(const render_graph::ResourceHandle* swap_chain_handle)
{
    // Get current frame index from swap chain
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

    return frame_index; // Return current frame index
}

// Create pipeline map for geometry pass
render_graph::GeometryPass::PipelineMap CreatePipelines()
{
    render_graph::GeometryPass::PipelineMap pipelines;

    // Create Lambert pipeline
    std::unique_ptr<render_graph::LambertPipeline> lambert_pipeline
        = std::make_unique<render_graph::LambertPipeline>();
    pipelines.emplace(render_graph::LambertMaterialTypeHandle::ID(), std::move(lambert_pipeline));
    
    // Create phong pipeline
    std::unique_ptr<render_graph::PhongPipeline> phong_pipeline
        = std::make_unique<render_graph::PhongPipeline>();
    pipelines.emplace(render_graph::PhongMaterialTypeHandle::ID(), std::move(phong_pipeline));

    return pipelines;
}

template <typename T>
bool CreateMaterial(
    AppContext& app_context, 
    render_graph::MaterialHandle* out_material_handle, render_graph::Material::SetupParam& setup_param)
{
    // Create Lambert material
    std::unique_ptr<T> material
        = std::make_unique<T>(*app_context.resource_adder, *app_context.resource_eraser);
    if (!material->Setup(setup_param))
        return false; // Failure

    // Add material to material container
    *out_material_handle = app_context.material_adder->AddMaterial(std::move(material));
    if (!out_material_handle->IsValid())
        return false; // Failure

    return true; // Success
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

} // namespace render_graph_test

TEST(Render, Template)
{
    bool result = false;

    /*******************************************************************************************************************
     * Initialize application
    /******************************************************************************************************************/

    // Application context
    render_graph_test::AppContext app_context;

    // Initialize render test
    result = render_graph_test::InitRenderTest(app_context);
    ASSERT_TRUE(result);

    /*******************************************************************************************************************
     * Initialize resources
    /******************************************************************************************************************/

    // Create swap chain resource
    render_graph::ResourceHandle swap_chain_handle = render_graph::ResourceHandle();
    result = render_graph_test::CreateSwapChain(app_context, &swap_chain_handle);
    ASSERT_TRUE(result);

    // Create command sets
    render_graph::CommandSetHandles command_set_handles;
    result = render_graph_test::CreateCommandSets(app_context, &command_set_handles);
    ASSERT_TRUE(result);

    /*******************************************************************************************************************
     * Initialize render graph
    /******************************************************************************************************************/

    // Initialize render pass id generator
    std::unique_ptr<render_graph::RenderPassIDGenerator> render_pass_id_generator 
        = std::make_unique<render_graph::RenderPassIDGenerator>();

    // Initialize render graph
    render_graph::RenderGraph render_graph;

    // TODO: Create render passes

    /*******************************************************************************************************************
     * Main loop and rendering
    /******************************************************************************************************************/

    result = render_graph_test::RunMessageLoop(
        app_context.running,
        [&]()
        {
            // Get current frame index
            UINT current_frame_index = render_graph_test::GetCurrentFrameIndex(&swap_chain_handle);

            // TODO: Add render passes here
            
            // Execute render graph
            result = render_graph_test::ExecuteRenderGraph(
                render_graph, &swap_chain_handle, &command_set_handles[current_frame_index]);
            if (!result)
                return false; // Stop on failure

            return true; // Continue running
        });

    /*******************************************************************************************************************
     * Cleanup application
    /******************************************************************************************************************/

    // Wait for gpu to finish
    dx12_util::WaitForGpu(
        dx12_util::Device::GetInstance().Get(), dx12_util::CommandQueue::GetInstance().Get());

    // Cleanup render test
    render_graph_test::CleanupRenderTest(app_context);
}

TEST(Render, Basic)
{
    // Co-initialize COM library
    HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
    ASSERT_TRUE(SUCCEEDED(hr));

    /*******************************************************************************************************************
     * Initialize application
    /******************************************************************************************************************/

    // Application context
    render_graph_test::AppContext app_context;

    // Initialize render test
    bool result = render_graph_test::InitRenderTest(app_context);
    ASSERT_TRUE(result);

    /*******************************************************************************************************************
     * Initialize resources
    /******************************************************************************************************************/

    // Create swap chain resource
    render_graph::ResourceHandle swap_chain_handle = render_graph::ResourceHandle();
    result = render_graph_test::CreateSwapChain(app_context, &swap_chain_handle);
    ASSERT_TRUE(result);

    // Create command sets
    render_graph::CommandSetHandles command_set_handles = render_graph::CommandSetHandles();
    result = render_graph_test::CreateCommandSets(app_context, &command_set_handles);
    ASSERT_TRUE(result);
    
    // Create empty texture
    render_graph::ResourceHandle empty_texture_handle = render_graph::ResourceHandle();
    {
        const uint32_t empty_texture_width = 1;
        const uint32_t empty_texture_height = 1;
        const DXGI_FORMAT empty_texture_format = DXGI_FORMAT_R8G8B8A8_UNORM;
        render_graph::HeapManager& heap_manager = render_graph::HeapManager::GetInstance();
        heap_manager.WithUniqueLock([&](render_graph::HeapManager& heap_manager)
        {
            result = render_graph_test::CreateTexture2D(
                app_context, &empty_texture_handle, empty_texture_width, empty_texture_height,
                DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_FLAG_NONE,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr,
                &empty_texture_format, nullptr, nullptr, nullptr,
                &render_graph::HeapManager::GetInstance().GetSrvHeapAllocator(), nullptr, nullptr);
            ASSERT_TRUE(result);
        });
    }

    // Load albedo texture
    ScratchImage albedo_image;
    TexMetadata albedo_metadata;
    result = render_graph_test::LoadPng(
        albedo_image, albedo_metadata, render_graph_test::MODEL_ALBEDO_TEXTURE_PATH);
    ASSERT_TRUE(result);

    // Create albedo texture resource
    render_graph::ResourceHandle albedo_texture_handle = render_graph::ResourceHandle();
    {
        render_graph::HeapManager& heap_manager = render_graph::HeapManager::GetInstance();
        heap_manager.WithUniqueLock([&](render_graph::HeapManager& heap_manager)
        {
            result = render_graph_test::CreateTexture2D(
                app_context,
                &albedo_texture_handle,
                static_cast<UINT>(albedo_metadata.width), static_cast<UINT>(albedo_metadata.height),
                albedo_metadata.format, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_FLAG_NONE,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr,
                &albedo_metadata.format, nullptr, nullptr, nullptr,
                &heap_manager.GetSrvHeapAllocator(), nullptr, nullptr);
        });
        ASSERT_TRUE(result);
    }

    // Create upload buffer for albedo texture
    bool albedo_texture_uploaded = false;
    render_graph::ResourceHandle albedo_texture_upload_buffer_handle = render_graph::ResourceHandle();
    result = render_graph_test::CreateBuffer(
            app_context, &albedo_texture_upload_buffer_handle, albedo_image.GetPixelsSize(), nullptr);
        ASSERT_TRUE(result);

    // Load normal texture
    ScratchImage normal_image;
    TexMetadata normal_metadata;
    result = render_graph_test::LoadPng(
        normal_image, normal_metadata, render_graph_test::MODEL_NORMAL_TEXTURE_PATH);
    ASSERT_TRUE(result);

    // Create normal texture resource
    render_graph::ResourceHandle normal_texture_handle = render_graph::ResourceHandle();
    {
        render_graph::HeapManager& heap_manager = render_graph::HeapManager::GetInstance();
        heap_manager.WithUniqueLock([&](render_graph::HeapManager& heap_manager)
        {
            result = render_graph_test::CreateTexture2D(
                app_context,
                &normal_texture_handle,
                static_cast<UINT>(normal_metadata.width), static_cast<UINT>(normal_metadata.height),
                normal_metadata.format, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_FLAG_NONE,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr,
                &normal_metadata.format, nullptr, nullptr, nullptr,
                &heap_manager.GetSrvHeapAllocator(), nullptr, nullptr);
        });
        ASSERT_TRUE(result);
    }

    // Create upload buffer for normal texture
    bool normal_texture_uploaded = false;
    render_graph::ResourceHandle normal_texture_upload_buffer_handle = render_graph::ResourceHandle();
    result = render_graph_test::CreateBuffer(
            app_context, &normal_texture_upload_buffer_handle, normal_image.GetPixelsSize(), nullptr);
        ASSERT_TRUE(result);

    // Load ao texture
    ScratchImage ao_image;
    TexMetadata ao_metadata;
    result = render_graph_test::LoadPng(
        ao_image, ao_metadata, render_graph_test::MODEL_AO_TEXTURE_PATH);
    ASSERT_TRUE(result);

    // Create ao texture resource
    render_graph::ResourceHandle ao_texture_handle = render_graph::ResourceHandle();
    {
        render_graph::HeapManager& heap_manager = render_graph::HeapManager::GetInstance();
        heap_manager.WithUniqueLock([&](render_graph::HeapManager& heap_manager)
        {
            result = render_graph_test::CreateTexture2D(
                app_context,
                &ao_texture_handle,
                static_cast<UINT>(ao_metadata.width), static_cast<UINT>(ao_metadata.height),
                ao_metadata.format, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_FLAG_NONE,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr,
                &ao_metadata.format, nullptr, nullptr, nullptr,
                &heap_manager.GetSrvHeapAllocator(), nullptr, nullptr);
        });
        ASSERT_TRUE(result);
    }

    // Create upload buffer for ao texture
    bool ao_texture_uploaded = false;
    render_graph::ResourceHandle ao_texture_upload_buffer_handle = render_graph::ResourceHandle();
    result = render_graph_test::CreateBuffer(
            app_context, &ao_texture_upload_buffer_handle, ao_image.GetPixelsSize(), nullptr);
        ASSERT_TRUE(result);

    // Load roughness texture
    ScratchImage roughness_image;
    TexMetadata roughness_metadata;
    result = render_graph_test::LoadPng(
        roughness_image, roughness_metadata, render_graph_test::MODEL_ROUGHNESS_TEXTURE_PATH);
    ASSERT_TRUE(result);

    // Create roughness texture resource
    render_graph::ResourceHandle roughness_texture_handle = render_graph::ResourceHandle();
    {
        render_graph::HeapManager& heap_manager = render_graph::HeapManager::GetInstance();
        heap_manager.WithUniqueLock([&](render_graph::HeapManager& heap_manager)
        {
            result = render_graph_test::CreateTexture2D(
                app_context,
                &roughness_texture_handle,
                static_cast<UINT>(roughness_metadata.width), static_cast<UINT>(roughness_metadata.height),
                roughness_metadata.format, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_FLAG_NONE,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr,
                &roughness_metadata.format, nullptr, nullptr, nullptr,
                &heap_manager.GetSrvHeapAllocator(), nullptr, nullptr);
        });
        ASSERT_TRUE(result);
    }

    // Create upload buffer for roughness texture
    bool roughness_texture_uploaded = false;
    render_graph::ResourceHandle roughness_texture_upload_buffer_handle = render_graph::ResourceHandle();
    result = render_graph_test::CreateBuffer(
            app_context, &roughness_texture_upload_buffer_handle, roughness_image.GetPixelsSize(), nullptr);
    ASSERT_TRUE(result);

    // Create gbuffer textures for each back buffer
    render_graph::ResourceHandles gbuffer_render_target_handles[render_graph_test::SWAP_CHAIN_BUFFER_COUNT];
    for (UINT back_buffer_index = 0; back_buffer_index < render_graph_test::SWAP_CHAIN_BUFFER_COUNT; ++back_buffer_index)
    {
        // Resize ResourceHandles
        gbuffer_render_target_handles[back_buffer_index].resize(
            (UINT)render_graph::geometry_pass::GBufferIndex::COUNT);

        for (UINT gbuffer_index = 0; gbuffer_index < (UINT)render_graph::geometry_pass::GBufferIndex::COUNT; ++gbuffer_index)
        {
            D3D12_CLEAR_VALUE clear_value = {};
            clear_value.Format = render_graph::geometry_pass::GBUFFER_FORMATS[gbuffer_index];
            clear_value.Color[0] = render_graph::geometry_pass::GBUFFER_CLEAR_COLORS[gbuffer_index][0];
            clear_value.Color[1] = render_graph::geometry_pass::GBUFFER_CLEAR_COLORS[gbuffer_index][1];
            clear_value.Color[2] = render_graph::geometry_pass::GBUFFER_CLEAR_COLORS[gbuffer_index][2];
            clear_value.Color[3] = render_graph::geometry_pass::GBUFFER_CLEAR_COLORS[gbuffer_index][3];

            render_graph::HeapManager& heap_manager = render_graph::HeapManager::GetInstance();
            heap_manager.WithUniqueLock([&](render_graph::HeapManager& heap_manager)
            {
                result = render_graph_test::CreateTexture2D(
                    app_context,
                    &gbuffer_render_target_handles[back_buffer_index][gbuffer_index],
                    app_context.client_width, app_context.client_height,
                    render_graph::geometry_pass::GBUFFER_FORMATS[gbuffer_index],
                    D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, 
                    D3D12_RESOURCE_STATE_RENDER_TARGET, &clear_value, 
                    nullptr, nullptr, nullptr, nullptr,
                    &heap_manager.GetSrvHeapAllocator(), &heap_manager.GetRtvHeapAllocator(), nullptr);
            });
            ASSERT_TRUE(result);
        }
    }

    // Create geometry's depth stencil texture
    render_graph::ResourceHandle gbuffer_depth_stencil_handles[render_graph_test::SWAP_CHAIN_BUFFER_COUNT];
    for (UINT back_buffer_index = 0; back_buffer_index < render_graph_test::SWAP_CHAIN_BUFFER_COUNT; ++back_buffer_index)
    {
        D3D12_CLEAR_VALUE clear_value = {};
        clear_value.Format = render_graph::geometry_pass::DEPTH_STENCIL_FORMAT;
        clear_value.DepthStencil.Depth = render_graph::geometry_pass::DEPTH_CLEAR_VALUE;
        clear_value.DepthStencil.Stencil = render_graph::geometry_pass::STENCIL_CLEAR_VALUE;

        render_graph::HeapManager& heap_manager = render_graph::HeapManager::GetInstance();
        heap_manager.WithUniqueLock([&](render_graph::HeapManager& heap_manager)
        {
            result = render_graph_test::CreateTexture2D(
                app_context,
                &gbuffer_depth_stencil_handles[back_buffer_index],
                app_context.client_width, app_context.client_height,
                render_graph::geometry_pass::DEPTH_STENCIL_FORMAT,
                D3D12_HEAP_TYPE_DEFAULT,
                D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, D3D12_RESOURCE_STATE_DEPTH_WRITE, 
                &clear_value, &render_graph::geometry_pass::DEPTH_STENCIL_SRV_FORMAT, // SRV format
                nullptr, // UAV format (same as texture format)
                nullptr, // RTV format (same as texture format)
                nullptr, // DSV format (same as texture format)
                &heap_manager.GetSrvHeapAllocator(), nullptr, &heap_manager.GetDsvHeapAllocator());
        });
        ASSERT_TRUE(result);
    }

    // Create full-screen triangle vertex buffer
    render_graph::ResourceHandle full_screen_triangle_vertex_buffer_handle;
    constexpr uint32_t FULL_SCREEN_TRIANGLE_VERTEX_COUNT = 3;
    {
        // Define full-screen triangle vertex data
        const render_graph::composition_pass::Vertex vertex_data[3] =
        {
            {XMFLOAT3(-1.0f, -1.0f, 0.0f)},
            {XMFLOAT3(-1.0f,  3.0f, 0.0f)},
            {XMFLOAT3( 3.0f, -1.0f, 0.0f)},
        };

        result = render_graph_test::CreateBuffer(
            app_context, &full_screen_triangle_vertex_buffer_handle, sizeof(vertex_data), vertex_data);
        ASSERT_TRUE(result);
    }

    // Create full-screen triangle index buffer
    render_graph::ResourceHandle full_screen_triangle_index_buffer_handle;
    {
        // Define full-screen triangle index data
        const uint32_t index_data[3] = {1, 2, 0};

        result = render_graph_test::CreateBuffer(
            app_context, &full_screen_triangle_index_buffer_handle, sizeof(index_data), index_data);
        ASSERT_TRUE(result);
    }

    // Create ImGui render target texture
    render_graph::ResourceHandle imgui_render_target_handles[render_graph_test::SWAP_CHAIN_BUFFER_COUNT];
    for (UINT back_buffer_index = 0; back_buffer_index < render_graph_test::SWAP_CHAIN_BUFFER_COUNT; ++back_buffer_index)
    {
        D3D12_CLEAR_VALUE clear_value = {};
        clear_value.Format = render_graph::imgui_pass::TARGET_FORMAT;
        clear_value.Color[0] = render_graph::imgui_pass::TARGET_CLEAR_COLOR[0];
        clear_value.Color[1] = render_graph::imgui_pass::TARGET_CLEAR_COLOR[1];
        clear_value.Color[2] = render_graph::imgui_pass::TARGET_CLEAR_COLOR[2];
        clear_value.Color[3] = render_graph::imgui_pass::TARGET_CLEAR_COLOR[3];

        render_graph::HeapManager& heap_manager = render_graph::HeapManager::GetInstance();
        heap_manager.WithUniqueLock([&](render_graph::HeapManager& heap_manager)
        {
            result = render_graph_test::CreateTexture2D(
                app_context,
                &imgui_render_target_handles[back_buffer_index],
                app_context.client_width, app_context.client_height,
                render_graph::imgui_pass::TARGET_FORMAT,
                D3D12_HEAP_TYPE_DEFAULT,
                D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_RESOURCE_STATE_RENDER_TARGET, &clear_value, 
                nullptr, nullptr, nullptr, nullptr,
                &heap_manager.GetSrvHeapAllocator(), &heap_manager.GetRtvHeapAllocator(), nullptr);
        });
        ASSERT_TRUE(result);
    }

    // Create Imgui context
    render_graph::ImguiContextHandle imgui_context_handle = render_graph::ImguiContextHandle();
    result = render_graph_test::CreateImguiContext(
        app_context, &imgui_context_handle,
        render_graph_test::CreateImGuiContext, render_graph_test::DestroyImguiContext);
    ASSERT_TRUE(result);

    // Create light structured buffer
    render_graph::ResourceHandle lights_buffer_handles[render_graph_test::SWAP_CHAIN_BUFFER_COUNT];
    for (UINT back_buffer_index = 0; back_buffer_index < render_graph_test::SWAP_CHAIN_BUFFER_COUNT; ++back_buffer_index)
    {
        render_graph::HeapManager::GetInstance().WithUniqueLock([&](render_graph::HeapManager& heap_manager)
        {
            // Get srv heap allocator
            dx12_util::DescriptorHeapAllocator& srv_heap_allocator = heap_manager.GetSrvHeapAllocator();

            result = render_graph_test::CreateStructuredBuffer(
                app_context, &lights_buffer_handles[back_buffer_index],
                sizeof(render_graph::Light::LightBuffer), render_graph_test::LIGHT_MAX_COUNT, &srv_heap_allocator);
                
            ASSERT_TRUE(result);
        });
    }

    // Create upload buffer for light structured buffer
    render_graph::ResourceHandle lights_upload_buffer_handles[render_graph_test::SWAP_CHAIN_BUFFER_COUNT];
    for (UINT back_buffer_index = 0; back_buffer_index < render_graph_test::SWAP_CHAIN_BUFFER_COUNT; ++back_buffer_index)
    {
        result = render_graph_test::CreateBuffer(
            app_context, &lights_upload_buffer_handles[back_buffer_index],
            sizeof(render_graph::Light::LightBuffer) * render_graph_test::LIGHT_MAX_COUNT, nullptr);
        ASSERT_TRUE(result);
    }

    // Create light config buffer
    render_graph::ResourceHandle light_config_buffer_handle = render_graph::ResourceHandle();
    {
        render_graph::Light::LightConfigBuffer light_config = {};
        light_config.num_lights = 0;

        result = render_graph_test::CreateBuffer(
            app_context, &light_config_buffer_handle, sizeof(render_graph::Light::LightConfigBuffer), &light_config);
        ASSERT_TRUE(result);
    }

    // Create final color texture for lighting pass
    render_graph::ResourceHandles final_color_texture_handles[render_graph_test::SWAP_CHAIN_BUFFER_COUNT];
    for (UINT back_buffer_index = 0; back_buffer_index < render_graph_test::SWAP_CHAIN_BUFFER_COUNT; ++back_buffer_index)
    {
        // Resize ResourceHandles
        final_color_texture_handles[back_buffer_index].resize((uint32_t)render_graph::lighting_pass::RenderTargetIndex::COUNT);
        for (uint32_t i = 0; i < (uint32_t)render_graph::lighting_pass::RenderTargetIndex::COUNT; ++i)
        {
            D3D12_CLEAR_VALUE clear_value = {};
            clear_value.Format = render_graph::lighting_pass::RENDER_TARGET_FORMATS[i];
            clear_value.Color[0] = render_graph::lighting_pass::RENDER_TARGET_CLEAR_COLORS[i][0];
            clear_value.Color[1] = render_graph::lighting_pass::RENDER_TARGET_CLEAR_COLORS[i][1];
            clear_value.Color[2] = render_graph::lighting_pass::RENDER_TARGET_CLEAR_COLORS[i][2];
            clear_value.Color[3] = render_graph::lighting_pass::RENDER_TARGET_CLEAR_COLORS[i][3];

            render_graph::HeapManager& heap_manager = render_graph::HeapManager::GetInstance();
            heap_manager.WithUniqueLock([&](render_graph::HeapManager& heap_manager)
            {
                result = render_graph_test::CreateTexture2D(
                    app_context,
                    &final_color_texture_handles[back_buffer_index][i],
                    app_context.client_width, app_context.client_height,
                    render_graph::lighting_pass::RENDER_TARGET_FORMATS[i],
                    D3D12_HEAP_TYPE_DEFAULT,
                    D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_RESOURCE_STATE_RENDER_TARGET, &clear_value, 
                    nullptr, nullptr, nullptr, nullptr,
                    &heap_manager.GetSrvHeapAllocator(), &heap_manager.GetRtvHeapAllocator(), nullptr);
            });
            ASSERT_TRUE(result);
        }
    }

    // Create shadow composition pass render target textures
    render_graph::ResourceHandles shadow_composition_render_target_handles[render_graph_test::SWAP_CHAIN_BUFFER_COUNT];
    for (UINT back_buffer_index = 0; back_buffer_index < render_graph_test::SWAP_CHAIN_BUFFER_COUNT; ++back_buffer_index)
    {
		// Resize ResourceHandles
        shadow_composition_render_target_handles[back_buffer_index].resize(
			(UINT)render_graph::shadow_composition_pass::RenderTargetIndex::COUNT);

        for (UINT rt_index = 0; rt_index < (UINT)render_graph::shadow_composition_pass::RenderTargetIndex::COUNT; ++rt_index)
        {
            D3D12_CLEAR_VALUE clear_value = {};
            clear_value.Format = render_graph::shadow_composition_pass::RENDER_TARGET_FORMATS[rt_index];
            clear_value.Color[0] = render_graph::shadow_composition_pass::RENDER_TARGET_CLEAR_COLORS[rt_index][0];
            clear_value.Color[1] = render_graph::shadow_composition_pass::RENDER_TARGET_CLEAR_COLORS[rt_index][1];
            clear_value.Color[2] = render_graph::shadow_composition_pass::RENDER_TARGET_CLEAR_COLORS[rt_index][2];
            clear_value.Color[3] = render_graph::shadow_composition_pass::RENDER_TARGET_CLEAR_COLORS[rt_index][3];

            render_graph::HeapManager& heap_manager = render_graph::HeapManager::GetInstance();
            heap_manager.WithUniqueLock([&](render_graph::HeapManager& heap_manager)
            {
                result = render_graph_test::CreateTexture2D(
                    app_context,
                    &shadow_composition_render_target_handles[back_buffer_index][rt_index],
                    app_context.client_width, app_context.client_height,
                    render_graph::shadow_composition_pass::RENDER_TARGET_FORMATS[rt_index],
                    D3D12_HEAP_TYPE_DEFAULT,
                    D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_RESOURCE_STATE_RENDER_TARGET, &clear_value, 
                    nullptr, nullptr, nullptr, nullptr,
                    &heap_manager.GetSrvHeapAllocator(), &heap_manager.GetRtvHeapAllocator(), nullptr);
            });
            ASSERT_TRUE(result);
        }
    }

    // Create shadow composition config buffer
    render_graph::ResourceHandle shadow_composition_config_buffer_handle = render_graph::ResourceHandle();
    {
        result = render_graph_test::CreateBuffer(
            app_context, &shadow_composition_config_buffer_handle, 
            sizeof(render_graph::shadow_composition_pass::ShadowCompositionConfigBuffer), nullptr);
        ASSERT_TRUE(result);
    }

    // Create camera
    {
        render_graph_test::CameraContext camera_context;

        // Create camera editor
        camera_context.camera_editor = std::make_unique<render_graph_test::CameraUIEditor>();

        // Create camera settings
        camera_context.camera_settings = std::make_unique<render_graph_test::CameraSetting>();
        render_graph_test::CameraSetting* camera_setting
            = dynamic_cast<render_graph_test::CameraSetting*>(camera_context.camera_settings.get());
        ASSERT_TRUE(camera_setting != nullptr);

        // Initialize camera settings
        camera_setting->name = "Main Camera";
        camera_setting->translation = XMFLOAT3(0.0f, 0.0f, -2.0f);
        camera_setting->rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
        camera_setting->fov_y = 60.0f;
        camera_setting->near_z = 0.1f;
        camera_setting->far_z = 100.0f;
        camera_setting->aspect_ratio = static_cast<float>(app_context.client_width) / static_cast<float>(app_context.client_height);

        // Create view-projection matrix buffer handle
        XMMATRIX identity_matrix = XMMatrixIdentity();
        result = render_graph_test::CreateBuffer(
            app_context, &camera_setting->view_proj_matrix_buffer_handle, sizeof(XMMATRIX), &identity_matrix);

        // Create inverse view-projection matrix buffer handle
        result = render_graph_test::CreateBuffer(
            app_context, &camera_setting->inv_view_proj_matrix_buffer_handle, sizeof(XMMATRIX), &identity_matrix);

        XMVECTOR up_vec = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        XMVECTOR forward_vec = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
        XMVECTOR rot_vec = XMQuaternionRotationRollPitchYaw(
            XMConvertToRadians(camera_setting->rotation.x),
            XMConvertToRadians(camera_setting->rotation.y),
            XMConvertToRadians(camera_setting->rotation.z));
        XMVECTOR rotated_forward_vec = XMVector3Rotate(forward_vec, rot_vec);
        XMVECTOR rotated_up_vec = XMVector3Rotate(up_vec, rot_vec);
        XMVECTOR position_vec = XMLoadFloat3(&camera_setting->translation);

        XMMATRIX view_matrix = XMMatrixLookAtLH(
            position_vec, XMVectorAdd(position_vec, rotated_forward_vec), rotated_up_vec);  

        // Calculate projection matrix
        XMMATRIX projection_matrix = XMMatrixPerspectiveFovLH(
            XMConvertToRadians(camera_setting->fov_y), camera_setting->aspect_ratio,
            camera_setting->near_z, camera_setting->far_z);

        // Combine view and projection matrices
        XMMATRIX view_proj_matrix = XMMatrixMultiply(view_matrix, projection_matrix);

        // Calculate inverse view-projection matrix
        XMMATRIX inv_view_proj_matrix = XMMatrixInverse(nullptr, view_proj_matrix);

        // Transpose matrix for HLSL
        camera_setting->view_proj_matrix = XMMatrixTranspose(view_proj_matrix);
        camera_setting->inv_view_proj_matrix = XMMatrixTranspose(inv_view_proj_matrix);

        // Update camera world matrix
        XMMATRIX translation_matrix = XMMatrixTranslationFromVector(position_vec);
        XMMATRIX rotation_matrix = XMMatrixRotationQuaternion(rot_vec);
        XMMATRIX camera_world_matrix = XMMatrixMultiply(rotation_matrix, translation_matrix);

        // Transpose camera world matrix for HLSL
        camera_setting->world_matrix = XMMatrixTranspose(camera_world_matrix);

        // Store camera context
        render_graph_test::g_camera_contexts[(uint32_t)render_graph_test::CameraID::MAIN_CAMERA] = std::move(camera_context);
    }

    // Marble bust lambert material
    {
        // Create material context
        render_graph_test::MaterialContext material_context;

        // Create lambert material editor
        material_context.material_editor = std::make_unique<render_graph_test::LambertMaterialEditor>();

        // Create lambert material settings
        material_context.material_settings = std::make_unique<render_graph_test::LambertMaterialEditor::Settings>();
        render_graph_test::LambertMaterialEditor::Settings* lambert_material_setting
            = dynamic_cast<render_graph_test::LambertMaterialEditor::Settings*>(material_context.material_settings.get());
        ASSERT_TRUE(lambert_material_setting != nullptr);

        // Initialize lambert material settings
        lambert_material_setting->name = "Marble Bust Lambert Material";
        lambert_material_setting->base_color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        lambert_material_setting->use_albedo_texture = true;
        lambert_material_setting->use_normal_texture = true;
        lambert_material_setting->use_ao_texture = true;
        lambert_material_setting->use_emissive_texture = false;
        lambert_material_setting->emissive_color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

        // Create setup param
        std::unique_ptr<render_graph::LambertMaterial::SetupParam> setup_param 
            = std::make_unique<render_graph::LambertMaterial::SetupParam>();
        setup_param->base_color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        setup_param->albedo_source = render_graph::LambertMaterial::ALBEDO_SOURCE_TEXTURE;
        setup_param->normal_source = render_graph::LambertMaterial::NORMAL_SOURCE_TEXTURE;
        setup_param->ao_source = render_graph::LambertMaterial::AO_SOURCE_TEXTURE;
        setup_param->emission_source = render_graph::LambertMaterial::EMISSION_SOURCE_NONE;
        setup_param->emission_color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
        
        setup_param->albedo_texture_handle = &albedo_texture_handle;
        setup_param->normal_texture_handle = &normal_texture_handle;
        setup_param->ao_texture_handle = &ao_texture_handle;
        setup_param->emission_texture_handle = &empty_texture_handle;

        // Create material
        result = render_graph_test::CreateMaterial<render_graph::LambertMaterial>(
            app_context, &lambert_material_setting->material_handle, *setup_param);
        ASSERT_TRUE(result);

        // Set material type handle id
        lambert_material_setting->material_type_handle_id = render_graph::LambertMaterialTypeHandle::ID();

        // Store material name
        render_graph_test::g_material_name_to_id_map[lambert_material_setting->name.c_str()] 
            = (uint32_t)render_graph_test::MaterialID::MARBLE_BUST_LAMBERT_MATERIAL;

        render_graph_test::g_material_names.emplace_back(lambert_material_setting->name.c_str());

        // Store material context
        render_graph_test::g_material_contexts[(uint32_t)render_graph_test::MaterialID::MARBLE_BUST_LAMBERT_MATERIAL] 
            = std::move(material_context);
    }

    // Marble bust phong material
    {
        // Create material context
        render_graph_test::MaterialContext material_context;

        // Create phong material editor
        material_context.material_editor = std::make_unique<render_graph_test::PhongMaterialEditor>();

        // Create phong material settings
        material_context.material_settings = std::make_unique<render_graph_test::PhongMaterialEditor::Settings>();
        render_graph_test::PhongMaterialEditor::Settings* phong_material_setting
            = dynamic_cast<render_graph_test::PhongMaterialEditor::Settings*>(material_context.material_settings.get());
        ASSERT_TRUE(phong_material_setting != nullptr);

        // Initialize phong material settings
        phong_material_setting->name = "Marble Bust Phong Material";
        phong_material_setting->base_color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        phong_material_setting->use_albedo_texture = true;
        phong_material_setting->use_normal_texture = true;
        phong_material_setting->use_ao_texture = true;
        phong_material_setting->use_specular_texture = false;
        phong_material_setting->use_roughness_texture = true;
        phong_material_setting->roughness = 1.0f;
        phong_material_setting->use_metallic_texture = false;
        phong_material_setting->metallic = 0.0f;
        phong_material_setting->use_emissive_texture = false;
        phong_material_setting->emissive_color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

        // Create setup param
        std::unique_ptr<render_graph::PhongMaterial::SetupParam> setup_param 
            = std::make_unique<render_graph::PhongMaterial::SetupParam>();
        setup_param->base_color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        setup_param->albedo_source = render_graph::PhongMaterial::ALBEDO_SOURCE_TEXTURE;
        setup_param->normal_source = render_graph::PhongMaterial::NORMAL_SOURCE_TEXTURE;
        setup_param->ao_source = render_graph::PhongMaterial::AO_SOURCE_TEXTURE;
        setup_param->specular_source = render_graph::PhongMaterial::SPECULAR_SOURCE_NONE;
        setup_param->roughness_source = render_graph::PhongMaterial::ROUGHNESS_SOURCE_TEXTURE;
        setup_param->roughness_value = 1.0f;
        setup_param->metalness_source = render_graph::PhongMaterial::METALNESS_SOURCE_VALUE;
        setup_param->metalness_value = 0.0f;
        setup_param->emission_source = render_graph::PhongMaterial::EMISSION_SOURCE_COLOR;
        setup_param->emission_color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

        setup_param->albedo_texture_handle = &albedo_texture_handle;
        setup_param->normal_texture_handle = &normal_texture_handle;
        setup_param->ao_texture_handle = &ao_texture_handle;
        setup_param->specular_texture_handle = &empty_texture_handle;
        setup_param->roughness_texture_handle = &roughness_texture_handle;
        setup_param->metalness_texture_handle = &empty_texture_handle;
        setup_param->emission_texture_handle = &empty_texture_handle;

        // Create material
        result = render_graph_test::CreateMaterial<render_graph::PhongMaterial>(
            app_context, &phong_material_setting->material_handle, *setup_param);
        ASSERT_TRUE(result);

        // Set material type handle id
        phong_material_setting->material_type_handle_id = render_graph::PhongMaterialTypeHandle::ID();

        // Store material name
        render_graph_test::g_material_name_to_id_map[phong_material_setting->name.c_str()] 
            = (uint32_t)render_graph_test::MaterialID::MARBLE_BUST_PHONG_MATERIAL;

        render_graph_test::g_material_names.emplace_back(phong_material_setting->name.c_str());

        // Store material context
        render_graph_test::g_material_contexts[(uint32_t)render_graph_test::MaterialID::MARBLE_BUST_PHONG_MATERIAL] 
            = std::move(material_context);
    }

    // Floor lambert material
    {
        // Create material context
        render_graph_test::MaterialContext material_context;

        // Create lambert material editor
        material_context.material_editor = std::make_unique<render_graph_test::LambertMaterialEditor>();

        // Create lambert material settings
        material_context.material_settings = std::make_unique<render_graph_test::LambertMaterialEditor::Settings>();
        render_graph_test::LambertMaterialEditor::Settings* lambert_material_setting
            = dynamic_cast<render_graph_test::LambertMaterialEditor::Settings*>(material_context.material_settings.get());
        ASSERT_TRUE(lambert_material_setting != nullptr);

        // Initialize lambert material settings
        lambert_material_setting->name = "Floor Lambert Material";
        lambert_material_setting->base_color = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
        lambert_material_setting->use_albedo_texture = false;
        lambert_material_setting->use_normal_texture = false;
        lambert_material_setting->use_ao_texture = false;
        lambert_material_setting->use_emissive_texture = false;
        lambert_material_setting->emissive_color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

        // Create setup param
        std::unique_ptr<render_graph::LambertMaterial::SetupParam> setup_param 
            = std::make_unique<render_graph::LambertMaterial::SetupParam>();
        setup_param->base_color = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
        setup_param->albedo_source = render_graph::LambertMaterial::ALBEDO_SOURCE_BASE_COLOR;
        setup_param->normal_source = render_graph::LambertMaterial::NORMAL_SOURCE_VERTEX;
        setup_param->ao_source = render_graph::LambertMaterial::AO_SOURCE_NONE;
        setup_param->emission_source = render_graph::LambertMaterial::EMISSION_SOURCE_NONE;
        setup_param->emission_color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

        setup_param->albedo_texture_handle = &empty_texture_handle;
        setup_param->normal_texture_handle = &empty_texture_handle;
        setup_param->ao_texture_handle = &empty_texture_handle;
        setup_param->emission_texture_handle = &empty_texture_handle;

        // Create material
        result = render_graph_test::CreateMaterial<render_graph::LambertMaterial>(
            app_context, &lambert_material_setting->material_handle, *setup_param);
        ASSERT_TRUE(result);

        // Set material type handle id
        lambert_material_setting->material_type_handle_id = render_graph::LambertMaterialTypeHandle::ID();

        // Store material name
        render_graph_test::g_material_name_to_id_map[lambert_material_setting->name.c_str()] 
            = (uint32_t)render_graph_test::MaterialID::FLOOR_LAMBERT_MATERIAL;
        render_graph_test::g_material_names.emplace_back(lambert_material_setting->name.c_str());

        // Store material context
        render_graph_test::g_material_contexts[(uint32_t)render_graph_test::MaterialID::FLOOR_LAMBERT_MATERIAL] 
            = std::move(material_context);
    }

    // Directional light
    {
        // Create light context
        render_graph_test::LightContext light_context;

        // Create directional light editor
        light_context.light_editor = std::make_unique<render_graph_test::DirectionalLightEditor>();

        // Create directional light settings
        light_context.light_settings = std::make_unique<render_graph_test::DirectionalLightEditor::Settings>();
        render_graph_test::DirectionalLightEditor::Settings* directional_light_setting
            = dynamic_cast<render_graph_test::DirectionalLightEditor::Settings*>(light_context.light_settings.get());
        ASSERT_TRUE(directional_light_setting != nullptr);

        // Initialize directional light settings
        directional_light_setting->name = "Directional Light";

        // Create directional light setup param
        std::unique_ptr<render_graph::DirectionalLight::SetupParam> setup_param 
            = std::make_unique<render_graph::DirectionalLight::SetupParam>();
        setup_param->color = directional_light_setting->color;
        setup_param->intensity = directional_light_setting->intensity;
        setup_param->distance = directional_light_setting->distance;
        setup_param->ortho_width = directional_light_setting->orthographic_width;
        setup_param->ortho_height = directional_light_setting->orthographic_height;
        setup_param->near_z = directional_light_setting->near_z;
        setup_param->far_z = directional_light_setting->far_z;
        setup_param->shadow_map_size = render_graph_test::DIRECTIONAL_LIGHT_SHADOW_MAP_SIZE;

        // Create directional light
        std::unique_ptr<render_graph::DirectionalLight> directional_light
            = std::make_unique<render_graph::DirectionalLight>();
        result = directional_light->Setup(*setup_param);
        ASSERT_TRUE(result);

        // Set transform of directional light
        directional_light->SetRotation(directional_light_setting->rotation);

        // Create view matrix buffer for shadow map rendering
        XMMATRIX identity_matrix = XMMatrixIdentity();
        render_graph::ResourceHandle light_view_matrix_buffer_handle = render_graph::ResourceHandle();
        result = render_graph_test::CreateBuffer(
            app_context, &light_view_matrix_buffer_handle, sizeof(XMMATRIX), &identity_matrix);
        ASSERT_TRUE(result);

        // Set view-projection matrix buffer to directional light
        directional_light->SetViewMatrixBufferHandle(light_view_matrix_buffer_handle);

        // Create projection matrix buffer for shadow map rendering
        render_graph::ResourceHandle light_proj_matrix_buffer_handle = render_graph::ResourceHandle();
        result = render_graph_test::CreateBuffer(
            app_context, &light_proj_matrix_buffer_handle, sizeof(XMMATRIX), &identity_matrix);
        ASSERT_TRUE(result);

        // Set projection matrix buffer to directional light
        directional_light->SetProjMatrixBufferHandle(light_proj_matrix_buffer_handle);

        // Create shadow map for directional light
        render_graph::HeapManager& heap_manager = render_graph::HeapManager::GetInstance();
        heap_manager.WithUniqueLock([&](render_graph::HeapManager& heap_manager)
        {
            for (uint32_t back_buffer_index = 0; back_buffer_index < render_graph_test::SWAP_CHAIN_BUFFER_COUNT; ++back_buffer_index)
            {
                D3D12_CLEAR_VALUE clear_value = {};
                clear_value.Format = render_graph::shadowing_pass::SHADOW_MAP_FORMAT;
                clear_value.DepthStencil.Depth = render_graph::shadowing_pass::SHADOW_MAP_CLEAR_VALUE;
                clear_value.DepthStencil.Stencil = render_graph::shadowing_pass::SHADOW_MAP_STENCIL_CLEAR_VALUE;

                render_graph::ResourceHandle shadow_map_handle = render_graph::ResourceHandle();
                result = render_graph_test::CreateTexture2D(
                    app_context, &shadow_map_handle,
                    render_graph_test::DIRECTIONAL_LIGHT_SHADOW_MAP_SIZE, render_graph_test::DIRECTIONAL_LIGHT_SHADOW_MAP_SIZE,
                    render_graph::shadowing_pass::SHADOW_MAP_FORMAT, D3D12_HEAP_TYPE_DEFAULT, 
                    D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clear_value,
                    &render_graph::shadowing_pass::SHADOW_MAP_SRV_FORMAT, // SRV format
                    nullptr, // UAV format (same as texture format)
                    nullptr, // RTV format (same as texture format)
                    nullptr, // DSV format (same as texture format)
                    &heap_manager.GetSrvHeapAllocator(), nullptr, &heap_manager.GetDsvHeapAllocator(),
                    L"Directional Light Shadow Map" + std::to_wstring(back_buffer_index));
                ASSERT_TRUE(result);

                // Store shadow map handle
                directional_light_setting->shadow_map_handles.push_back(shadow_map_handle);
            }

            // Set shadow map to directional light
            directional_light->SetShadowMapHandles(directional_light_setting->shadow_map_handles);
        });

        // Update matrices
        directional_light->UpdateWorldMatrices();
        directional_light->UpdateViewProjMatrix();

        // Add light to light container
        directional_light_setting->light_handle = app_context.light_adder->AddLight(std::move(directional_light));
        ASSERT_TRUE(directional_light_setting->light_handle.IsValid());

        // Store light type handle id
        directional_light_setting->light_type_handle_id = render_graph::DirectionalLightTypeHandle::ID();

        // Store light context
        render_graph_test::g_light_contexts[(uint32_t)render_graph_test::LightID::DIRECTIONAL_LIGHT] = std::move(light_context);
    }

    // Ambient light
    {
        // Create light context
        render_graph_test::LightContext light_context;

        // Create ambient light editor
        light_context.light_editor = std::make_unique<render_graph_test::AmbientLightEditor>();

        // Create ambient light settings
        light_context.light_settings = std::make_unique<render_graph_test::LightSetting>();
        render_graph_test::LightSetting* ambient_light_setting
            = dynamic_cast<render_graph_test::LightSetting*>(light_context.light_settings.get());
        ASSERT_TRUE(ambient_light_setting != nullptr);

        // Initialize ambient light settings
        ambient_light_setting->name = "Ambient Light";
        ambient_light_setting->color = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);

        // Create ambient light setup param
        std::unique_ptr<render_graph::AmbientLight::SetupParam> setup_param 
            = std::make_unique<render_graph::AmbientLight::SetupParam>();
        setup_param->color = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);

        // Create ambient light
        std::unique_ptr<render_graph::AmbientLight> ambient_light
            = std::make_unique<render_graph::AmbientLight>();
        result = ambient_light->Setup(*setup_param);
        ASSERT_TRUE(result);

        // Update matrices
        ambient_light->UpdateWorldMatrices();
        ambient_light->UpdateViewProjMatrix();

        // Add light to light container
        ambient_light_setting->light_handle = app_context.light_adder->AddLight(std::move(ambient_light));
        ASSERT_TRUE(ambient_light_setting->light_handle.IsValid());

        // Store light type handle id
        ambient_light_setting->light_type_handle_id = render_graph::AmbientLightTypeHandle::ID();

        // Store light context
        render_graph_test::g_light_contexts[(uint32_t)render_graph_test::LightID::AMBIENT_LIGHT] = std::move(light_context);
    }

    // Point light
    {
        // Create light context
        render_graph_test::LightContext light_context;

        // Create point light editor
        light_context.light_editor = std::make_unique<render_graph_test::PointLightEditor>();

        // Create point light settings
        light_context.light_settings = std::make_unique<render_graph_test::PointLightEditor::Settings>();
        render_graph_test::PointLightEditor::Settings* point_light_setting
            = dynamic_cast<render_graph_test::PointLightEditor::Settings*>(light_context.light_settings.get());
        ASSERT_TRUE(point_light_setting != nullptr);

        // Initialize point light settings
        point_light_setting->name = "Point Light";
        point_light_setting->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        point_light_setting->intensity = 1.0f;
        point_light_setting->range = 5.0f;
        point_light_setting->translation = XMFLOAT3(2.0f, 2.0f, -2.0f);

        // Create point light setup param
        std::unique_ptr<render_graph::PointLight::SetupParam> setup_param 
            = std::make_unique<render_graph::PointLight::SetupParam>();
        setup_param->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        setup_param->intensity = 1.0f;
        setup_param->range = 5.0f;

        // Create point light
        std::unique_ptr<render_graph::PointLight> point_light
            = std::make_unique<render_graph::PointLight>();
        result = point_light->Setup(*setup_param);
        ASSERT_TRUE(result);

        // Set transform of point light
        point_light->SetPosition(point_light_setting->translation);

        // Update matrices
        point_light->UpdateWorldMatrices();
        point_light->UpdateViewProjMatrix();

        // Add light to light container
        point_light_setting->light_handle = app_context.light_adder->AddLight(std::move(point_light));
        ASSERT_TRUE(point_light_setting->light_handle.IsValid());

        // Store light type handle id
        point_light_setting->light_type_handle_id = render_graph::PointLightTypeHandle::ID();

        // Store light context
        render_graph_test::g_light_contexts[(uint32_t)render_graph_test::LightID::POINT_LIGHT] = std::move(light_context);
    }

    // Marble bust object
    {
        // Create object context
        render_graph_test::ObjectContext object_context;

        // Create object editor
        object_context.object_editor = std::make_unique<render_graph_test::ObjectEditor>();

        // Create object settings
        object_context.object_settings = std::make_unique<render_graph_test::ObjectSetting>();
        render_graph_test::ObjectSetting* object_setting
            = dynamic_cast<render_graph_test::ObjectSetting*>(object_context.object_settings.get());
        ASSERT_TRUE(object_setting != nullptr);

        // Initialize object settings
        object_setting->name = "Marble Bust Object";
        object_setting->translation = XMFLOAT3(0.0f, -0.8f, 0.0f);
        object_setting->rotation = XMFLOAT3(0.0f, 180.0f, 0.0f);
        object_setting->scale = XMFLOAT3(0.03f, 0.03f, 0.03f);
        object_setting->material_id = (uint32_t)render_graph_test::MaterialID::MARBLE_BUST_LAMBERT_MATERIAL;

        // Load mfm file
        fpos_t mfm_file_size = 0;
        std::unique_ptr<uint8_t[]> mfm_file_data = utility_header::LoadFile(
            render_graph_test::MODEL_FILE_PATH, mfm_file_size);
        ASSERT_TRUE(mfm_file_data != nullptr);

        // Create mfm object
        std::unique_ptr<mono_forge_model::MFM> mfm 
            = std::make_unique<mono_forge_model::MFM>(std::move(mfm_file_data), static_cast<uint32_t>(mfm_file_size));

        // Iterate through mesh nodes and create meshes
        for (uint32_t material_index = 0; material_index < mfm->GetMeshHeader()->material_count; ++material_index)
        {
            // Get mesh node
            const mono_forge_model::MFMMeshNode* mesh_node = mfm->GetMeshNode(material_index);

            // Get material name
            const char* material_name = mfm->GetMaterialName(material_index);

            // Create mesh struct
            render_graph_test::Mesh mesh;

            // Create vertex buffer for the material
            result = render_graph_test::CreateBuffer(
                app_context, &mesh.vertex_buffer_handle, mesh_node->vertex_size * mesh_node->vertex_count,
                mfm->GetVertexData(material_index));
            ASSERT_TRUE(result);

            // Create index buffer for the material
            result = render_graph_test::CreateBuffer(
                app_context, &mesh.index_buffer_handle, mesh_node->index_size * mesh_node->index_count,
                mfm->GetIndexData(material_index));
            ASSERT_TRUE(result);

            // Store index count
            mesh.index_count = mesh_node->index_count;

            // Get marble bust lambert material context
            render_graph_test::MaterialContext& phong_material_context
                = render_graph_test::g_material_contexts[
                    (uint32_t)render_graph_test::MaterialID::MARBLE_BUST_PHONG_MATERIAL];

            // Get material setting
            render_graph_test::MaterialSetting* material_setting
                = dynamic_cast<render_graph_test::MaterialSetting*>(phong_material_context.material_settings.get());
            assert(material_setting != nullptr && "Failed to get material setting");

            // Assign material
            mesh.material_handle = material_setting->material_handle;

            // Add mesh to object context
            object_setting->meshes.emplace_back(std::move(mesh));
        }

        // Create world matrix buffer for object
        result = render_graph_test::CreateBuffer(
            app_context, &object_setting->world_buffer_handle, sizeof(render_graph::geometry_pass::WorldBuffer), nullptr);
        ASSERT_TRUE(result);

        XMVECTOR translation_vec = XMLoadFloat3(&object_setting->translation);
        XMVECTOR rotation_vec = XMQuaternionRotationRollPitchYaw(
            XMConvertToRadians(object_setting->rotation.x),
            XMConvertToRadians(object_setting->rotation.y),
            XMConvertToRadians(object_setting->rotation.z));
        XMVECTOR scale_vec = XMLoadFloat3(&object_setting->scale);

        XMMATRIX translation_matrix = XMMatrixTranslationFromVector(translation_vec);
        XMMATRIX rotation_matrix = XMMatrixRotationQuaternion(rotation_vec);
        XMMATRIX scale_matrix = XMMatrixScalingFromVector(scale_vec);

        object_setting->world_buffer.world_matrix = scale_matrix * rotation_matrix * translation_matrix;
        object_setting->world_buffer.world_inverse_transpose 
            = XMMatrixTranspose(XMMatrixInverse(nullptr, object_setting->world_buffer.world_matrix));
        
        // Transpose world matrix for HLSL
        object_setting->world_buffer.world_matrix = XMMatrixTranspose(object_setting->world_buffer.world_matrix);
        object_setting->world_buffer.world_inverse_transpose = XMMatrixTranspose(object_setting->world_buffer.world_inverse_transpose);

        // Store object context
        render_graph_test::g_object_contexts[(uint32_t)render_graph_test::ObjectID::MARBLE_BUST_OBJECT] = std::move(object_context);
    }

    // Floor object
    {
        // Create object context
        render_graph_test::ObjectContext object_context;

        // Create object editor
        object_context.object_editor = std::make_unique<render_graph_test::ObjectEditor>();

        // Create object settings
        object_context.object_settings = std::make_unique<render_graph_test::ObjectSetting>();
        render_graph_test::ObjectSetting* object_setting
            = dynamic_cast<render_graph_test::ObjectSetting*>(object_context.object_settings.get());
        ASSERT_TRUE(object_setting != nullptr);

        // Initialize object settings
        object_setting->name = "Floor Object";
        object_setting->translation = XMFLOAT3(0.0f, -1.0f, 0.0f);
        object_setting->rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
        object_setting->scale = XMFLOAT3(3.0f, 3.0f, 3.0f);
        object_setting->material_id = (uint32_t)render_graph_test::MaterialID::FLOOR_LAMBERT_MATERIAL;

        // Load mfm file
        fpos_t mfm_file_size = 0;
        std::unique_ptr<uint8_t[]> mfm_file_data = utility_header::LoadFile(
            render_graph_test::FLOOR_MODEL_FILE_PATH, mfm_file_size);
        ASSERT_TRUE(mfm_file_data != nullptr);

        // Create mfm object
        std::unique_ptr<mono_forge_model::MFM> mfm 
            = std::make_unique<mono_forge_model::MFM>(std::move(mfm_file_data), static_cast<uint32_t>(mfm_file_size));

        // Iterate through mesh nodes and create meshes
        for (uint32_t material_index = 0; material_index < mfm->GetMeshHeader()->material_count; ++material_index)
        {
            // Get mesh node
            const mono_forge_model::MFMMeshNode* mesh_node = mfm->GetMeshNode(material_index);

            // Get material name
            const char* material_name = mfm->GetMaterialName(material_index);

            // Create mesh struct
            render_graph_test::Mesh mesh;

            // Create vertex buffer for the material
            result = render_graph_test::CreateBuffer(
                app_context, &mesh.vertex_buffer_handle, mesh_node->vertex_size * mesh_node->vertex_count,
                mfm->GetVertexData(material_index));
            ASSERT_TRUE(result);

            // Create index buffer for the material
            result = render_graph_test::CreateBuffer(
                app_context, &mesh.index_buffer_handle, mesh_node->index_size * mesh_node->index_count,
                mfm->GetIndexData(material_index));
            ASSERT_TRUE(result);

            // Store index count
            mesh.index_count = mesh_node->index_count;

            // Get floor lambert material context
            render_graph_test::MaterialContext& lambert_material_context
                = render_graph_test::g_material_contexts[
                    (uint32_t)render_graph_test::MaterialID::FLOOR_LAMBERT_MATERIAL];

            // Get  material setting
            render_graph_test::MaterialSetting* material_setting
                = dynamic_cast<render_graph_test::MaterialSetting*>(lambert_material_context.material_settings.get());
            assert(material_setting != nullptr && "Failed to get material setting");

            // Assign material
            mesh.material_handle = material_setting->material_handle;

            // Add mesh to object context
            object_setting->meshes.emplace_back(std::move(mesh));
        }

        // Create world matrix buffer for object
        result = render_graph_test::CreateBuffer(
            app_context, &object_setting->world_buffer_handle, sizeof(render_graph::geometry_pass::WorldBuffer), nullptr);
        ASSERT_TRUE(result);

        XMVECTOR translation_vec = XMLoadFloat3(&object_setting->translation);
        XMVECTOR rotation_vec = XMQuaternionRotationRollPitchYaw(
            XMConvertToRadians(object_setting->rotation.x),
            XMConvertToRadians(object_setting->rotation.y),
            XMConvertToRadians(object_setting->rotation.z));
        XMVECTOR scale_vec = XMLoadFloat3(&object_setting->scale);

        XMMATRIX translation_matrix = XMMatrixTranslationFromVector(translation_vec);
        XMMATRIX rotation_matrix = XMMatrixRotationQuaternion(rotation_vec);
        XMMATRIX scale_matrix = XMMatrixScalingFromVector(scale_vec);

        object_setting->world_buffer.world_matrix = scale_matrix * rotation_matrix * translation_matrix;
        object_setting->world_buffer.world_inverse_transpose 
            = XMMatrixTranspose(XMMatrixInverse(nullptr, object_setting->world_buffer.world_matrix));

        // Transpose world matrix for HLSL
        object_setting->world_buffer.world_matrix = XMMatrixTranspose(object_setting->world_buffer.world_matrix);
        object_setting->world_buffer.world_inverse_transpose = XMMatrixTranspose(object_setting->world_buffer.world_inverse_transpose);

        // Store object context
        render_graph_test::g_object_contexts[(uint32_t)render_graph_test::ObjectID::FLOOR_OBJECT] = std::move(object_context);
    }

    /*******************************************************************************************************************
     * Initialize render graph
    /******************************************************************************************************************/

    // Initialize render pass id generator
    std::unique_ptr<render_graph::RenderPassIDGenerator> render_pass_id_generator 
        = std::make_unique<render_graph::RenderPassIDGenerator>();

    // Initialize render graph
    render_graph::RenderGraph render_graph;

    // Initialize buffer update pass
    std::unique_ptr<render_graph::BufferUploadPass> buffer_update_pass 
        = render_graph::BufferUploadPass::CreateInstance<render_graph::BufferUploadPass>();
    ASSERT_NE(buffer_update_pass, nullptr);

    // Initialize geometry pass
    std::unique_ptr<render_graph::GeometryPass> geometry_pass = nullptr;
    {
        // Create pipelines for geometry pass
        std::unordered_map<render_graph::MaterialTypeHandleID, std::unique_ptr<render_graph::Pipeline>> pipelines 
            = render_graph_test::CreatePipelines();

        // Create geometry pass
        geometry_pass 
            = render_graph::GeometryPass::CreateInstance<render_graph::GeometryPass>(std::move(pipelines));
        ASSERT_NE(geometry_pass, nullptr);
    }

    // Initialize composition pass
    std::unique_ptr<render_graph::CompositionPass> composition_pass = nullptr;
    {
        // Create composition pipeline
        std::unique_ptr<render_graph::Pipeline> composition_pipeline 
            = std::make_unique<render_graph::CompositionPipeline>();
        ASSERT_TRUE(result);

        // Create composition pass
        composition_pass 
            = render_graph::CompositionPass::CreateInstance<render_graph::CompositionPass>(std::move(composition_pipeline));
    }
    ASSERT_NE(composition_pass, nullptr);

    // Initialize imgui pass
    std::unique_ptr<render_graph::ImguiPass> imgui_pass 
        = render_graph::ImguiPass::CreateInstance<render_graph::ImguiPass>();
    ASSERT_NE(imgui_pass, nullptr);

    // Initialize texture upload pass
    std::unique_ptr<render_graph::TextureUploadPass> texture_upload_pass 
        = render_graph::TextureUploadPass::CreateInstance<render_graph::TextureUploadPass>();
    ASSERT_NE(texture_upload_pass, nullptr);

    // Initialize light upload pass
    std::unique_ptr<render_graph::LightUploadPass> light_upload_pass 
        = render_graph::LightUploadPass::CreateInstance<render_graph::LightUploadPass>();
    ASSERT_NE(light_upload_pass, nullptr);

    // Initialize lighting pass
    std::unique_ptr<render_graph::LightingPass> lighting_pass = nullptr;
    {
        // Create lighting pipeline
        std::unique_ptr<render_graph::Pipeline> lighting_pipeline 
            = std::make_unique<render_graph::LightingPipeline>();
        ASSERT_TRUE(result);

        // Create lighting pass
        lighting_pass 
            = render_graph::LightingPass::CreateInstance<render_graph::LightingPass>(std::move(lighting_pipeline));
        ASSERT_NE(lighting_pass, nullptr);
    }

    // Initialize shadowing pass
    std::unique_ptr<render_graph::ShadowingPass> shadowing_pass = nullptr;
    {
        // Create shadowing pipeline
        std::unique_ptr<render_graph::Pipeline> shadowing_pipeline 
            = std::make_unique<render_graph::ShadowingPipeline>();
        ASSERT_TRUE(result);

        // Create shadowing pass
        shadowing_pass 
            = render_graph::ShadowingPass::CreateInstance<render_graph::ShadowingPass>(std::move(shadowing_pipeline));
        ASSERT_NE(shadowing_pass, nullptr);
    }

    // Initialize shadow composition pass
    std::unique_ptr<render_graph::ShadowCompositionPass> shadow_composition_pass = nullptr;
    {
        // Create shadow composition pipeline
        std::unique_ptr<render_graph::Pipeline> shadow_composition_pipeline 
            = std::make_unique<render_graph::ShadowCompositionPipeline>();
        ASSERT_TRUE(result);

        // Create shadow composition pass
        shadow_composition_pass 
            = render_graph::ShadowCompositionPass::CreateInstance<render_graph::ShadowCompositionPass>(
                std::move(shadow_composition_pipeline));
        ASSERT_NE(shadow_composition_pass, nullptr);
    }

    /*******************************************************************************************************************
     * Main loop and rendering
    /******************************************************************************************************************/

    // Prepare clear color
    const float clear_color[4] = {0.0f, 0.0f, 0.0f, 1.0f};

    // Prepare view port
    D3D12_VIEWPORT view_port = {};
    view_port.Width = static_cast<float>(app_context.client_width);
    view_port.Height = static_cast<float>(app_context.client_height);
    view_port.MinDepth = 0.0f;
    view_port.MaxDepth = 1.0f;
    view_port.TopLeftX = 0.0f;
    view_port.TopLeftY = 0.0f;

    // Prepare scissor rect
    D3D12_RECT scissor_rect = {};
    scissor_rect.top = 0;
    scissor_rect.bottom = app_context.client_height;
    scissor_rect.left = 0;
    scissor_rect.right = app_context.client_width;

    result = render_graph_test::RunMessageLoop(
        app_context.running,
        [&]() -> bool
        {
            /***********************************************************************************************************
             * Update data
            /**********************************************************************************************************/

            // Get current frame index
            UINT frame_index = render_graph_test::GetCurrentFrameIndex(&swap_chain_handle);

            /***********************************************************************************************************
             * Light upload pass
            /**********************************************************************************************************/

            {
                for (auto& [id, light_context] : render_graph_test::g_light_contexts)
                {
                    // Get light settings
                    render_graph_test::LightSetting* light_settings
                        = dynamic_cast<render_graph_test::LightSetting*>(light_context.light_settings.get());
                    assert(light_settings != nullptr && "Failed to get light setting.");

                    // Add upload light if active
                    if (light_settings->active_self)
                        light_upload_pass->AddUploadLight(&light_settings->light_handle);
                }

                // Set lights upload buffer handle
                light_upload_pass->SetLightsUploadBufferHandle(&lights_upload_buffer_handles[frame_index]);

                // Set light config buffer handle
                light_upload_pass->SetLightConfigBufferHandle(&light_config_buffer_handle);

                // Get camera context
                render_graph_test::CameraContext& camera_context 
                    = render_graph_test::g_camera_contexts[(uint32_t)render_graph_test::CameraID::MAIN_CAMERA];

                // Get camera setting
                render_graph_test::CameraSetting* camera_setting
                    = dynamic_cast<render_graph_test::CameraSetting*>(camera_context.camera_settings.get());
                assert(camera_setting != nullptr && "Failed to get camera setting.");

                // Set light config
                render_graph::Light::LightConfigBuffer light_config = {};
                light_config.screen_size.x = static_cast<float>(app_context.client_width);
                light_config.screen_size.y = static_cast<float>(app_context.client_height);
                light_config.max_lights = render_graph_test::LIGHT_MAX_COUNT;
                light_config.shadow_intensity = render_graph_test::g_shadow_intensity;
                light_config.camera_world_matrix = camera_setting->world_matrix;
                light_config.lambert_material_id = render_graph::LambertMaterialTypeHandle::ID();
                light_config.phong_material_id = render_graph::PhongMaterialTypeHandle::ID();
                light_config.directional_light_id = render_graph::DirectionalLightTypeHandle::ID();
                light_config.ambient_light_id = render_graph::AmbientLightTypeHandle::ID();
                light_config.point_light_id = render_graph::PointLightTypeHandle::ID();
                light_upload_pass->SetLightConfig(std::move(light_config));

                // Add light upload pass to render graph
                light_upload_pass->AddToGraph(render_graph);
            }

            /***********************************************************************************************************
             * Buffer update pass
            /**********************************************************************************************************/

            {
                for (auto& [id, camera_context] : render_graph_test::g_camera_contexts)
                {
                    // Get camera settings
                    render_graph_test::CameraSetting* camera_settings
                        = dynamic_cast<render_graph_test::CameraSetting*>(camera_context.camera_settings.get());
                    assert(camera_settings != nullptr && "Failed to get camera setting.");

                    if (!camera_settings->active_self)
                        continue;

                    // Add view-proj matrix buffer update task
                    buffer_update_pass->AddUploadTask(
                        render_graph::BufferUploadPass::UploadTask{
                            &camera_settings->view_proj_matrix_buffer_handle, 
                            &camera_settings->view_proj_matrix, sizeof(XMMATRIX)});

                    // Add inverse view-proj matrix buffer update task
                    buffer_update_pass->AddUploadTask(
                        render_graph::BufferUploadPass::UploadTask{
                            &camera_settings->inv_view_proj_matrix_buffer_handle, 
                            &camera_settings->inv_view_proj_matrix, sizeof(XMMATRIX)});
                }

                for (auto& [id, object_context] : render_graph_test::g_object_contexts)
                {
                    // Get object settings
                    render_graph_test::ObjectSetting* object_settings
                        = dynamic_cast<render_graph_test::ObjectSetting*>(object_context.object_settings.get());
                    assert(object_settings != nullptr && "Failed to get object setting.");

                    if (!object_settings->active_self)
                        continue;

                    // Add world matrix buffer update task
                    buffer_update_pass->AddUploadTask(
                        render_graph::BufferUploadPass::UploadTask{
                            &object_settings->world_buffer_handle, 
                            &object_settings->world_buffer, sizeof(render_graph::geometry_pass::WorldBuffer)});
                }

                for (auto& [id, material_context] : render_graph_test::g_material_contexts)
                {
                    // Get material settings
                    render_graph_test::MaterialSetting* material_settings
                        = dynamic_cast<render_graph_test::MaterialSetting*>(material_context.material_settings.get());
                    assert(material_settings != nullptr && "Failed to get material setting.");

                    // Get material
                    render_graph::MaterialManager::GetInstance().WithLock([&](render_graph::MaterialManager& material_manager)
                    {
                        render_graph::Material& material = material_manager.GetMaterial(&material_settings->material_handle);

                        uint32_t data_size = 0;
                        const void* material_data = material.GetBufferData(data_size);

                        // Add buffer update task
                        buffer_update_pass->AddUploadTask(
                            render_graph::BufferUploadPass::UploadTask{
                                material.GetBufferHandle(), material_data, data_size});
                    });
                }

                for (auto& [id, light_context] : render_graph_test::g_light_contexts)
                {
                    // Get light settings
                    render_graph_test::LightSetting* light_settings
                        = dynamic_cast<render_graph_test::LightSetting*>(light_context.light_settings.get());
                    assert(light_settings != nullptr && "Failed to get light setting.");

                    if (!light_settings->active_self)
                        continue;

                    // Get light
                    render_graph::LightManager::GetInstance().WithLock([&](render_graph::LightManager& light_manager)
                    {
                        // Get light
                        render_graph::Light& light = light_manager.GetLight(&light_settings->light_handle);

                        // Only directional light needs view-proj matrix update in this test
                        if (light.GetLightTypeHandleID() != render_graph::DirectionalLightTypeHandle::ID())
                            return;

                        // Add view matrix buffer update task
                        buffer_update_pass->AddUploadTask(
                            render_graph::BufferUploadPass::UploadTask{
                                light.GetViewMatrixBufferHandle(), &light.GetViewMatrix(), sizeof(XMMATRIX)});

                        // Add proj matrix buffer update task
                        buffer_update_pass->AddUploadTask(
                            render_graph::BufferUploadPass::UploadTask{
                                light.GetProjMatrixBufferHandle(), &light.GetProjMatrix(), sizeof(XMMATRIX)});
                    });
                }

                // Set shadow composition config buffer
                render_graph::shadow_composition_pass::ShadowCompositionConfigBuffer shadow_composition_config = {};
                shadow_composition_config.screen_size
                    = XMFLOAT2(static_cast<float>(app_context.client_width), static_cast<float>(app_context.client_height));
                shadow_composition_config.shadow_bias = render_graph_test::g_shadow_bias;
                shadow_composition_config.slope_scaled_bias = render_graph_test::g_shadow_slope_scaled_bias;
                shadow_composition_config.slope_bias_exponent = render_graph_test::g_shadow_slope_bias_exponent;
                shadow_composition_pass->SetShadowCompositionConfig(std::move(shadow_composition_config));

                // Add shadow composition config buffer update task
                buffer_update_pass->AddUploadTask(
                    render_graph::BufferUploadPass::UploadTask{
                        &shadow_composition_config_buffer_handle, &shadow_composition_pass->GetShadowCompositionConfig(),
                        sizeof(render_graph::shadow_composition_pass::ShadowCompositionConfigBuffer)});

                // Add lights buffer update task
                buffer_update_pass->AddUploadTask(
                    render_graph::BufferUploadPass::StructuredBufferUploadTask{
                        &lights_buffer_handles[frame_index], &lights_upload_buffer_handles[frame_index], 
                        sizeof(render_graph::Light::LightBuffer) * render_graph_test::LIGHT_MAX_COUNT});

                // Add buffer update pass to render graph
                buffer_update_pass->AddToGraph(render_graph);
            }

            /***********************************************************************************************************
             * Texture upload pass
            /**********************************************************************************************************/

            {
                if (!albedo_texture_uploaded)
                {
                    // Add texture upload task for albedo texture
                    render_graph::TextureUploadPass::UploadTask texture_upload_task = {};
                    texture_upload_task.texture_handle = &albedo_texture_handle;
                    texture_upload_task.upload_buffer_handle = &albedo_texture_upload_buffer_handle;
                    texture_upload_task.data = albedo_image.GetPixels();
                    texture_upload_pass->AddUploadTask(std::move(texture_upload_task));

                    albedo_texture_uploaded = true;
                }

                if (!normal_texture_uploaded)
                {
                    // Add texture upload task for normal texture
                    render_graph::TextureUploadPass::UploadTask texture_upload_task = {};
                    texture_upload_task.texture_handle = &normal_texture_handle;
                    texture_upload_task.upload_buffer_handle = &normal_texture_upload_buffer_handle;
                    texture_upload_task.data = normal_image.GetPixels();
                    texture_upload_pass->AddUploadTask(std::move(texture_upload_task));

                    normal_texture_uploaded = true;
                }

                if (!ao_texture_uploaded)
                {
                    // Add texture upload task for ao texture
                    render_graph::TextureUploadPass::UploadTask texture_upload_task = {};
                    texture_upload_task.texture_handle = &ao_texture_handle;
                    texture_upload_task.upload_buffer_handle = &ao_texture_upload_buffer_handle;
                    texture_upload_task.data = ao_image.GetPixels();
                    texture_upload_pass->AddUploadTask(std::move(texture_upload_task));

                    ao_texture_uploaded = true;
                }

                if (!roughness_texture_uploaded)
                {
                    // Add texture upload task for roughness texture
                    render_graph::TextureUploadPass::UploadTask texture_upload_task = {};
                    texture_upload_task.texture_handle = &roughness_texture_handle;
                    texture_upload_task.upload_buffer_handle = &roughness_texture_upload_buffer_handle;
                    texture_upload_task.data = roughness_image.GetPixels();
                    texture_upload_pass->AddUploadTask(std::move(texture_upload_task));

                    roughness_texture_uploaded = true;
                }

                // Add texture upload pass to render graph
                texture_upload_pass->AddToGraph(render_graph);
            }

            /***********************************************************************************************************
             * Shadowing pass
            /**********************************************************************************************************/

            {
                for (auto& [id, light_context] : render_graph_test::g_light_contexts)
                {
                    // Get light settings
                    render_graph_test::LightSetting* light_settings
                        = dynamic_cast<render_graph_test::LightSetting*>(light_context.light_settings.get());
                    assert(light_settings != nullptr && "Failed to get light setting.");

                    if (!light_settings->active_self)
                        continue; // Skip if not active

                    // Only directional light casts shadow in this test
                    if (light_settings->light_type_handle_id != render_graph::DirectionalLightTypeHandle::ID())
                        continue;

                    // Add shadow casting light
                    shadowing_pass->AddShadowCastingLight(&light_settings->light_handle);
                }

                for (auto& [id, object_context] : render_graph_test::g_object_contexts)
                {
                    // Get object settings
                    render_graph_test::ObjectSetting* object_settings
                        = dynamic_cast<render_graph_test::ObjectSetting*>(object_context.object_settings.get());
                    assert(object_settings != nullptr && "Failed to get object setting.");

                    if (!object_settings->active_self)
                        continue; // Skip if not active

                    for (const render_graph_test::Mesh& mesh : object_settings->meshes)
                    {
                        // Add shadow casting mesh info
                        render_graph::ShadowingPass::MeshInfo mesh_info = {};
                        mesh_info.vertex_buffer_handle = &mesh.vertex_buffer_handle;
                        mesh_info.index_buffer_handle = &mesh.index_buffer_handle;
                        mesh_info.index_count = mesh.index_count;
                        mesh_info.world_matrix_buffer_handle = &object_settings->world_buffer_handle;
                        shadowing_pass->AddShadowCasterMeshInfo(std::move(mesh_info));
                    }
                }

                // Set current frame index
                shadowing_pass->SetCurrentFrameIndex(frame_index);

                // Add shadowing pass to render graph
                shadowing_pass->AddToGraph(render_graph);
            }

            /***********************************************************************************************************
             * Geometry pass
            /**********************************************************************************************************/

            {
                // Set gbuffer textures
                geometry_pass->SetGBuffers(&gbuffer_render_target_handles[frame_index]);

                // Set depth stencil texture
                geometry_pass->SetDepthStencil(&gbuffer_depth_stencil_handles[frame_index]);

                // For each mesh in the model
                for (const auto& [id, object_context] : render_graph_test::g_object_contexts)
                {
                    // Get object settings
                    render_graph_test::ObjectSetting* object_settings
                        = dynamic_cast<render_graph_test::ObjectSetting*>(object_context.object_settings.get());
                    assert(object_settings != nullptr && "Failed to get object setting.");

                    if (!object_settings->active_self)
                        continue; // Skip if not active

                    for (const render_graph_test::Mesh& mesh : object_settings->meshes)
                    {
                        // Add draw mesh info
                        render_graph::GeometryPass::MeshInfo mesh_info = {};
                        mesh_info.vertex_buffer_handle = &mesh.vertex_buffer_handle;
                        mesh_info.index_buffer_handle = &mesh.index_buffer_handle;
                        mesh_info.index_count = mesh.index_count;
                        mesh_info.material_handle = &mesh.material_handle;
                        mesh_info.world_matrix_buffer_handle = &object_settings->world_buffer_handle;
                        geometry_pass->AddDrawMeshInfo(std::move(mesh_info));
                    }
                }

                // Get main camera context
                render_graph_test::CameraContext& camera_context 
                    = render_graph_test::g_camera_contexts[(uint32_t)render_graph_test::CameraID::MAIN_CAMERA];

                // Get camera setting
                render_graph_test::CameraSetting* camera_setting
                    = dynamic_cast<render_graph_test::CameraSetting*>(camera_context.camera_settings.get());
                assert(camera_setting != nullptr && "Failed to get camera setting.");

                // Set view-projection matrix buffer
                geometry_pass->SetViewProjMatrixBuffer(&camera_setting->view_proj_matrix_buffer_handle);

                // Set view portcamera_setting
                geometry_pass->SetViewport(view_port);

                // Set scissor rect
                geometry_pass->SetScissorRect(scissor_rect);

                // Add geometry pass to render graph
                geometry_pass->AddToGraph(render_graph);
            }

            /***********************************************************************************************************
             * Shadow composition pass
            /**********************************************************************************************************/

            {
                // Set full-screen triangle info
                render_graph::ShadowCompositionPass::FullScreenTriangleInfo fs_triangle_info = {};
                fs_triangle_info.vertex_buffer_handle = &full_screen_triangle_vertex_buffer_handle;
                fs_triangle_info.index_buffer_handle = &full_screen_triangle_index_buffer_handle;
                fs_triangle_info.index_count = FULL_SCREEN_TRIANGLE_VERTEX_COUNT;
                shadow_composition_pass->SetFullScreenTriangleInfo(std::move(fs_triangle_info));

                // Add shadow casting lights
                for (auto& [id, light_context] : render_graph_test::g_light_contexts)
                {
                    // Get light settings
                    render_graph_test::LightSetting* light_settings
                        = dynamic_cast<render_graph_test::LightSetting*>(light_context.light_settings.get());
                    assert(light_settings != nullptr && "Failed to get light setting.");

                    if (!light_settings->active_self)
                        continue; // Skip if not active

                    // Only directional light casts shadow in this test
                    if (light_settings->light_type_handle_id != render_graph::DirectionalLightTypeHandle::ID())
                        continue;

                    // Add shadow casting light
                    shadow_composition_pass->AddShadowCastingLight(&light_settings->light_handle);
                }

                // Set shadow composition pass render target textures
                shadow_composition_pass->SetRenderTargetHandles(&shadow_composition_render_target_handles[frame_index]);

                // Set shadow composition config buffer handle
                shadow_composition_pass->SetShadowCompositionConfigBufferHandle(&shadow_composition_config_buffer_handle);

                // Get main camera context
                render_graph_test::CameraContext& camera_context 
                    = render_graph_test::g_camera_contexts[(uint32_t)render_graph_test::CameraID::MAIN_CAMERA];

                // Get camera setting
                render_graph_test::CameraSetting* camera_setting
                    = dynamic_cast<render_graph_test::CameraSetting*>(camera_context.camera_settings.get());
                assert(camera_setting != nullptr && "Failed to get camera setting.");

                // Set inverse view-projection matrix buffer
                shadow_composition_pass->SetCameraInvViewProjMatrixBufferHandle(
                    &camera_setting->inv_view_proj_matrix_buffer_handle);

                // Set depth texture handle
                shadow_composition_pass->SetDepthTextureHandle(&gbuffer_depth_stencil_handles[frame_index]);

                // Set normal texture handle from gbuffer
                shadow_composition_pass->SetNormalTextureHandle(
                    &gbuffer_render_target_handles[frame_index][(UINT)render_graph::geometry_pass::GBufferIndex::NORMAL]);

                // Set view port
                shadow_composition_pass->SetViewport(view_port);

                // Set scissor rect
                shadow_composition_pass->SetScissorRect(scissor_rect);

                // Add shadow composition pass to render graph
                shadow_composition_pass->AddToGraph(render_graph);
            }

            /***********************************************************************************************************
             * Lighting pass
            /**********************************************************************************************************/

            {
                // Set full-screen triangle info
                render_graph::LightingPass::FullScreenTriangleInfo fs_triangle_info = {};
                fs_triangle_info.vertex_buffer_handle = &full_screen_triangle_vertex_buffer_handle;
                fs_triangle_info.index_buffer_handle = &full_screen_triangle_index_buffer_handle;
                fs_triangle_info.index_count = FULL_SCREEN_TRIANGLE_VERTEX_COUNT;
                lighting_pass->SetFullScreenTriangleInfo(std::move(fs_triangle_info));

                // Set view port
                lighting_pass->SetViewport(view_port);

                // Set scissor rect
                lighting_pass->SetScissorRect(scissor_rect);

                // Set final color textures
                lighting_pass->SetRenderTargetTextureHandles(&final_color_texture_handles[frame_index]);

                // Get main camera context
                render_graph_test::CameraContext& camera_context 
                    = render_graph_test::g_camera_contexts[(uint32_t)render_graph_test::CameraID::MAIN_CAMERA];

                // Get camera setting
                render_graph_test::CameraSetting* camera_setting
                    = dynamic_cast<render_graph_test::CameraSetting*>(camera_context.camera_settings.get());
                assert(camera_setting != nullptr && "Failed to get camera setting.");
                    
                // Set inverse view-projection matrix buffer
                lighting_pass->SetInvViewProjMatrixBufferHandle(&camera_setting->inv_view_proj_matrix_buffer_handle);

                // Set light config buffer handle
                lighting_pass->SetLightConfigBufferHandle(&light_config_buffer_handle);

                // Set lights buffer handle
                lighting_pass->SetLightsBufferHandle(&lights_buffer_handles[frame_index]);

                // Set albedo texture handle from gbuffer
                lighting_pass->SetAlbedoTextureHandle(
                    &gbuffer_render_target_handles[frame_index][(UINT)render_graph::geometry_pass::GBufferIndex::ALBEDO]);

                // Set normal texture handle from gbuffer
                lighting_pass->SetNormalTextureHandle(
                    &gbuffer_render_target_handles[frame_index][(UINT)render_graph::geometry_pass::GBufferIndex::NORMAL]);

                // Set metalness texture handle from gbuffer
                lighting_pass->SetMetalnessTextureHandle(
                    &gbuffer_render_target_handles[frame_index][(UINT)render_graph::geometry_pass::GBufferIndex::METALNESS]);

                // Set roughness texture handle from gbuffer
                lighting_pass->SetRoughnessTextureHandle(
                    &gbuffer_render_target_handles[frame_index][(UINT)render_graph::geometry_pass::GBufferIndex::ROUGHNESS]);

                // Set specular texture handle from gbuffer
                lighting_pass->SetSpecularTextureHandle(
                    &gbuffer_render_target_handles[frame_index][(UINT)render_graph::geometry_pass::GBufferIndex::SPECULAR]);

                // Set ao texture handle from gbuffer
                lighting_pass->SetAOTextureHandle(
                    &gbuffer_render_target_handles[frame_index][(UINT)render_graph::geometry_pass::GBufferIndex::AO]);

                // Set emission texture handle from gbuffer
                lighting_pass->SetEmissionTextureHandle(
                    &gbuffer_render_target_handles[frame_index][(UINT)render_graph::geometry_pass::GBufferIndex::EMISSION]);

                // Set mask material texture handle from gbuffer
                lighting_pass->SetMaskMaterialTextureHandle(
                    &gbuffer_render_target_handles[frame_index][(UINT)render_graph::geometry_pass::GBufferIndex::MASK_MATERIAL]);

                // Set mask shadow texture handle from shadow composition pass
                lighting_pass->SetMaskShadowTextureHandle(
                    &shadow_composition_render_target_handles[frame_index][(UINT)render_graph::shadow_composition_pass::RenderTargetIndex::SHADOW_MASK]);

                // Set depth stencil texture from gbuffer
                lighting_pass->SetDepthStencilTextureHandle(&gbuffer_depth_stencil_handles[frame_index]);

                // Add lighting pass to render graph
                lighting_pass->AddToGraph(render_graph);
            }

            /***********************************************************************************************************
             * Imgui pass
            /**********************************************************************************************************/

            // Set target for imgui pass
            imgui_pass->SetTargetTexture(&imgui_render_target_handles[frame_index]);

            // Set draw function for imgui pass
            imgui_pass->SetDrawFunc(render_graph_test::DrawUI);

            // Set imgui context handle
            imgui_pass->SetImguiContext(&imgui_context_handle);

            // Add imgui pass to render graph
            imgui_pass->AddToGraph(render_graph);

            /***********************************************************************************************************
             * Composition pass
            /**********************************************************************************************************/

            // Set target for composition pass
            composition_pass->SetTargetSwapChain(&swap_chain_handle, clear_color);

            // Set post-process texture
            composition_pass->SetPostProcessTexture(
                &final_color_texture_handles[frame_index][(uint32_t)render_graph::lighting_pass::RenderTargetIndex::FINAL_COLOR]);

            // Set ui texture
            composition_pass->SetUITexture(&imgui_render_target_handles[frame_index]);

            // Set full-screen triangle info
            {
                render_graph::CompositionPass::FullScreenTriangleInfo fs_triangle_info = {};
                fs_triangle_info.vertex_buffer_handle = &full_screen_triangle_vertex_buffer_handle;
                fs_triangle_info.index_buffer_handle = &full_screen_triangle_index_buffer_handle;
                fs_triangle_info.index_count = FULL_SCREEN_TRIANGLE_VERTEX_COUNT;
                composition_pass->SetFullScreenTriangleInfo(std::move(fs_triangle_info));
            }

            // Set view port
            composition_pass->SetViewport(view_port);

            // Set scissor rect
            composition_pass->SetScissorRect(scissor_rect);

            // Add composition pass to render graph
            composition_pass->AddToGraph(render_graph);

            /***********************************************************************************************************
             * Execute render graph
            /**********************************************************************************************************/

            result = render_graph_test::ExecuteRenderGraph(
                render_graph, &swap_chain_handle, &command_set_handles[frame_index]);
            if (!result)
                return false; // Stop on failure

            return true; // Continue running
        });

    /*******************************************************************************************************************
     * Cleanup application
    /******************************************************************************************************************/

    // Wait for gpu to finish
    dx12_util::WaitForGpu(
        dx12_util::Device::GetInstance().Get(), dx12_util::CommandQueue::GetInstance().Get());

    // Cleanup render passes
    buffer_update_pass.reset();
    geometry_pass.reset();
    composition_pass.reset();
    imgui_pass.reset();

    // Cleanup render test
    render_graph_test::CleanupRenderTest(app_context);

    // Co-uninitialize COM library
    CoUninitialize();
}