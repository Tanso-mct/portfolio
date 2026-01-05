#include "mono_forge_app_template/src/pch.h"
#include "mono_forge_app_template/include/initial_entities.h"

#include "imgui/include/imgui.h"
#include "imgui/include/imgui_internal.h"
#include "imgui/include/imgui_impl_win32.h"
#include "imgui/include/imgui_impl_dx12.h"
#include "imgui/include/ImGuizmo.h"

#include "mono_meta_extension/include/meta_component.h"
#include "mono_scene_extension/include/scene_tag_component.h"
#include "mono_window_extension/include/window_component.h"
#include "mono_graphics_extension/include/window_render_bind_component.h"
#include "mono_scene_extension/include/scene_component.h"
#include "mono_graphics_extension/include/camera_component.h"
#include "mono_graphics_extension/include/renderable_component.h"
#include "mono_graphics_extension/include/ui_component.h"
#include "mono_graphics_extension/include/directional_light_component.h"
#include "mono_graphics_extension/include/ambient_light_component.h"
#include "mono_graphics_extension/include/point_light_component.h"

#include "mono_window_service/include/window_service.h"
#include "mono_graphics_service/include/graphics_service.h"
#include "mono_graphics_extension/include/graphics_system.h"

#include "mono_forge_app_template/include/initial_scene.h"

namespace mono_forge_app_template
{

bool CreateInitialEntities(
    ecs::World& ecs_world, mono_service::ServiceProxyManager& service_proxy_manager)
{
    // Initial window
    ecs::Entity window_entity = CreateInitialWindowEntity(ecs_world, service_proxy_manager);
    if (!window_entity.IsValid())
        return false; // Failed to create initial window entity

    // Initial scene
    ecs::Entity scene_entity = CreateInitialSceneEntity(ecs_world, service_proxy_manager, window_entity);
    if (!scene_entity.IsValid())
        return false; // Failed to create initial scene entity

    return true; // Success
}

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

ImGuiContext* CreateImGuiContext(HWND hwnd)
{
    bool result = false;

    // Get main monitor scale
    ImGui_ImplWin32_EnableDpiAwareness();
    float main_scale 
        = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

     // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGuiContext* context = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = nullptr; // Disable saving .ini file
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    // Setup Dear ImGui font
    ImFont* font 
        = io.Fonts->AddFontFromFileTTF(
            WINDOW_FONT_PATH, WINDOW_FONT_SIZE * main_scale, nullptr, io.Fonts->GetGlyphRangesJapanese());
    io.FontDefault = font;

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
    init_info.NumFramesInFlight = mono_graphics_extension::BACK_BUFFER_COUNT;
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
	ImGuizmo::SetImGuiContext(context);

    // Shutdown ImGui
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
};

ecs::Entity CreateInitialWindowEntity(
    ecs::World& ecs_world, mono_service::ServiceProxyManager& service_proxy_manager)
{
    ecs::Entity window_entity = ecs_world.CreateEntity();

    // Meta component
    {
        std::unique_ptr<mono_meta_extension::MetaComponent::SetupParam> param
            = std::make_unique<mono_meta_extension::MetaComponent::SetupParam>();
        param->name = WINDOW_ENTITY_NAME;

        if (!ecs_world.AddComponent<mono_meta_extension::MetaComponent>(
            window_entity, mono_meta_extension::MetaComponentHandle::ID(), std::move(param)))
            return ecs::Entity();
    }

    // Scene tag component
    {
        std::unique_ptr<mono_scene_extension::SceneTagComponent::SetupParam> param
            = std::make_unique<mono_scene_extension::SceneTagComponent::SetupParam>();
        param->scene_id = InitialScene::ID();

        if (!ecs_world.AddComponent<mono_scene_extension::SceneTagComponent>(
            window_entity, mono_scene_extension::SceneTagComponentHandle::ID(), std::move(param)))
            return ecs::Entity();
    }

    // Window component
    {
        std::unique_ptr<mono_window_extension::WindowComponent::SetupParam> param
            = std::make_unique<mono_window_extension::WindowComponent::SetupParam>();
        param->title = WINDOW_NAME;
        param->class_name = WINDOW_CLASS_NAME;
        param->window_proc = WndProc;

        // Make process DPI aware and obtain main monitor scale
        ImGui_ImplWin32_EnableDpiAwareness();
        float main_scale 
            = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

        param->width = WINDOW_WIDTH * main_scale;
        param->height = WINDOW_HEIGHT * main_scale;

        std::unique_ptr<mono_service::ServiceProxy> window_service_proxy = nullptr;
        service_proxy_manager.WithLock([&](mono_service::ServiceProxyManager& manager)
        {
            // Clone the proxy for component usage
            window_service_proxy = manager.GetServiceProxy(mono_window_service::WindowServiceHandle::ID()).Clone();
        });

        if (!ecs_world.AddComponent<mono_window_extension::WindowComponent>(
            window_entity, mono_window_extension::WindowComponentHandle::ID(),
            std::move(param), std::move(window_service_proxy)))
            return ecs::Entity();
    }

    // Window render bind component
    {
        std::unique_ptr<mono_graphics_extension::WindowRenderBindComponent::SetupParam> param
            = std::make_unique<mono_graphics_extension::WindowRenderBindComponent::SetupParam>();
        param->imgui_create_func = CreateImGuiContext;
        param->imgui_destroy_func = DestroyImguiContext;

        std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy = nullptr;
        service_proxy_manager.WithLock([&](mono_service::ServiceProxyManager& manager)
        {
            // Clone the proxy for component usage
            graphics_service_proxy = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID()).Clone();
        });

        if (!ecs_world.AddComponent<mono_graphics_extension::WindowRenderBindComponent>(
            window_entity, mono_graphics_extension::WindowRenderBindComponentHandle::ID(),
            std::move(param), std::move(graphics_service_proxy)))
            return ecs::Entity();
    }

	return window_entity; // Success
}

ecs::Entity CreateInitialSceneEntity(
    ecs::World& ecs_world, mono_service::ServiceProxyManager& service_proxy_manager,
    const ecs::Entity& window_entity)
{
    // Initial scene
    ecs::Entity entity = ecs_world.CreateEntity();

    // Meta component
    {
        std::unique_ptr<mono_meta_extension::MetaComponent::SetupParam> param
            = std::make_unique<mono_meta_extension::MetaComponent::SetupParam>();
        param->name = SCENE_ENTITY_NAME;

        if (!ecs_world.AddComponent<mono_meta_extension::MetaComponent>(
            entity, mono_meta_extension::MetaComponentHandle::ID(), std::move(param)))
            return ecs::Entity();
    }

    // Scene component
    {
        std::unique_ptr<mono_scene_extension::SceneComponent::SetupParam> param
            = std::make_unique<mono_scene_extension::SceneComponent::SetupParam>();

        // Set required window component IDs
        param->required_window_component_ids = { 
            mono_window_extension::WindowComponentHandle::ID(), 
            mono_graphics_extension::WindowRenderBindComponentHandle::ID()};

        // Set required camera component IDs
        param->required_camera_component_ids = { 
            mono_graphics_extension::CameraComponentHandle::ID() };
        
        // Set required renderable component IDs
        param->required_renderable_component_ids = { 
            mono_graphics_extension::RenderableComponentHandle::ID() };

        // Set required UI component IDs
        param->required_ui_component_ids = {
            mono_graphics_extension::UIComponentHandle::ID() };

        // Set light component IDs
        param->light_component_ids = {
            mono_graphics_extension::DirectionalLightComponentHandle::ID(),
            mono_graphics_extension::AmbientLightComponentHandle::ID(),
            mono_graphics_extension::PointLightComponentHandle::ID()};

        // Set max light count
        param->max_light_count = mono_forge_app_template::MAX_LIGHT_COUNT;

        // Set initial state to NeedsLoad
        param->initial_state = mono_scene_extension::SceneState::NeedsLoad;

        // Create initial scene and add to scene map
        std::unique_ptr<mono_scene_extension::Scene> scene
            = std::make_unique<InitialScene>(
                service_proxy_manager, entity, window_entity);

        if (!ecs_world.AddComponent<mono_scene_extension::SceneComponent>(
            entity, mono_scene_extension::SceneComponentHandle::ID(), std::move(param), 
            InitialScene::ID(), std::move(scene)))
            return ecs::Entity();
    }

	return entity; // Success
}

} // namespace mono_forge_app_template