#include "mono_forge/src/pch.h"

using namespace DirectX;

#include "utility_header/logger.h"

#include "mono_asset_service/include/asset_service.h"
#include "mono_transform_service/include/transform_service.h"
#include "mono_window_service/include/window_service.h"
#include "mono_graphics_service/include/graphics_service.h"
#include "mono_entity_archive_service/include/entity_archive_service.h"

#include "mono_forge/include/assets.h"
#include "mono_forge/include/materials.h"
#include "mono_forge/include/components.h"
#include "mono_forge/include/systems.h"
#include "mono_forge/include/initial_scene.h"
#include "mono_forge/include/initial_entities.h"
#include "mono_forge/include/service_hub.h"
#include "mono_forge/include/ecs_hub.h"

namespace mono_forge
{

// Main function return codes
constexpr int MAIN_ERROR = -1;
constexpr int MAIN_SUCCESS = 0;

// Run message loop
bool RunMessageLoop(bool& running, std::function<bool()> frame_func);

// Initialize service and return service hub
std::unique_ptr<ServiceHub> InitializeService();

// Initialize ECS and return ECS hub
std::unique_ptr<mono_forge::ECSHub> InitializeECS(mono_service::ServiceProxyManager& service_proxy_manager);

} // namespace mono_forge

#ifdef _DEBUG
int main()
#else
int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
#endif
{
    // Initialize COM library
    HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
    if (FAILED(hr))
    {
        utility_header::ConsoleLogErr({ "Failed to initialize COM library!" }, __FILE__, __LINE__, __FUNCTION__);
        return mono_forge::MAIN_ERROR; // COM initialization failed
    }

    // Initialize service
    std::unique_ptr<mono_forge::ServiceHub> service_hub = mono_forge::InitializeService();
    if (!service_hub)
    {
        utility_header::ConsoleLogErr({ "Service initialization failed!" }, __FILE__, __LINE__, __FUNCTION__);
        return mono_forge::MAIN_ERROR; // Service initialization failed
    }

    // Initialize ECS
    std::unique_ptr<mono_forge::ECSHub> ecs_hub = mono_forge::InitializeECS(service_hub->GetServiceProxyManager());
    if (!ecs_hub)
    {
        utility_header::ConsoleLogErr({ "ECS initialization failed!" }, __FILE__, __LINE__, __FUNCTION__);
        return mono_forge::MAIN_ERROR; // ECS initialization failed
    }

    // Create initial entities and components
    if (!mono_forge::CreateInitialEntities(ecs_hub->GetWorld(), service_hub->GetServiceProxyManager()))
    {
        utility_header::ConsoleLogErr({ "Initial entity creation failed!" }, __FILE__, __LINE__, __FUNCTION__);
        return mono_forge::MAIN_ERROR; // Initial entity creation failed
    }

    // Run message loop
    bool is_runnning = true;
    mono_forge::RunMessageLoop(is_runnning, [&]() -> bool
    {
        // Update systems
        if (!ecs_hub->Update(mono_forge::g_system_execution_order))
            return false;

        // Update services
        return service_hub->Update();
    });

    // Cleanup
    ecs_hub.reset();
    service_hub.reset();

    // Uninitialize COM library
    CoUninitialize();

    return mono_forge::MAIN_SUCCESS;
}

namespace mono_forge
{

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

std::unique_ptr<ServiceHub> InitializeService()
{
    std::unique_ptr<ServiceHub> service_hub = std::make_unique<ServiceHub>();

    // Import asset service
    {
        mono_asset_service::AssetService::SetupParam param;
        if (!service_hub->Import<
            mono_asset_service::AssetService, mono_asset_service::AssetServiceHandle>(
                mono_forge::AssetThreadAffinityHandle::ID(), param))
            return nullptr;
    }

    // Import transform service
    {
        mono_transform_service::TransformService::SetupParam param;
        if (!service_hub->Import<
            mono_transform_service::TransformService, mono_transform_service::TransformServiceHandle>(
                mono_forge::MainThreadAffinityHandle::ID(), param))
            return nullptr;
    }

    // Import window service
    {
        mono_window_service::WindowService::SetupParam param;
        if (!service_hub->Import<
            mono_window_service::WindowService, mono_window_service::WindowServiceHandle>(
                mono_forge::MainThreadAffinityHandle::ID(), param))
            return nullptr;
    }

    // Import graphics service
    {
        mono_graphics_service::GraphicsService::SetupParam param;
        if (!service_hub->Import<
            mono_graphics_service::GraphicsService, mono_graphics_service::GraphicsServiceHandle>(
                mono_forge::MainThreadAffinityHandle::ID(), param))
            return nullptr;
    }

    // Import entity archive service
    {
        mono_entity_archive_service::EntityArchiveService::SetupParam param;

        // Set component name map
        param.component_name_map = std::move(g_component_name_map);

        // Set component adder map
        param.component_adder_map = std::move(g_component_adder_map);

        // Set component reflection registry
        param.component_reflection_registry = std::move(g_component_reflection_registry);

        // Set setup param field type registry
        param.setup_param_field_type_registry_ = std::move(g_setup_param_field_type_registry);

        // Set component field value setter
        param.setup_param_field_value_setter = std::move(g_component_field_value_setter);

        // Set material setup param editor registry
        param.material_setup_param_editor_registry = std::move(g_material_setup_param_editor_registry);

        if (!service_hub->Import<
            mono_entity_archive_service::EntityArchiveService, mono_entity_archive_service::EntityArchiveServiceHandle>(
                mono_forge::MainThreadAffinityHandle::ID(), param))
            return nullptr;
    }

    // Set service update order
    service_hub->SetUpdateOrder({
        mono_asset_service::AssetServiceHandle::ID(),
        mono_transform_service::TransformServiceHandle::ID(),
        mono_window_service::WindowServiceHandle::ID(),
        mono_graphics_service::GraphicsServiceHandle::ID(),
        mono_entity_archive_service::EntityArchiveServiceHandle::ID()});

    return service_hub;
}

std::unique_ptr<ECSHub> InitializeECS(mono_service::ServiceProxyManager& service_proxy_manager)
{
    std::unique_ptr<mono_forge::ECSHub> ecs_hub = std::make_unique<mono_forge::ECSHub>();

    service_proxy_manager.WithLock([&](mono_service::ServiceProxyManager& manager)
    {
        // Register systems using global registrations
        for (const auto& registration_func : g_system_registrations)
            registration_func(*ecs_hub, manager);
    });

    {
        // Create component descriptor registry from global registry
        std::unique_ptr<ecs::ComponentDescriptorRegistry> component_descriptor_registry
            = std::make_unique<ecs::ComponentDescriptorRegistry>(std::move(g_component_descriptor_registry));

        // Create the ecs world
        ecs_hub->CreateWorld(std::move(component_descriptor_registry));
    }

    return ecs_hub;
}

} // namespace mono_forge