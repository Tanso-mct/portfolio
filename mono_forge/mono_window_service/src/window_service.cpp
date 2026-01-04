#include "mono_window_service/src/pch.h"
#include "mono_window_service/include/window_service.h"

#include "mono_window_service/include/window_service_command_list.h"
#include "mono_window_service/include/window_service_view.h"

namespace mono_window_service
{

WindowService::WindowService(mono_service::ServiceThreadAffinityID thread_affinity_id) :
    mono_service::Service(thread_affinity_id)
{
}

WindowService::~WindowService()
{
    /*******************************************************************************************************************
     * Window provider
    /******************************************************************************************************************/

    window_eraser_.reset();
    window_adder_.reset();
    window_manager_.reset();
    window_container_.reset();
}

bool WindowService::Setup(mono_service::Service::SetupParam& param)
{
    // Call base class Setup
    if (!mono_service::Service::Setup(param))
        return false;

    /*******************************************************************************************************************
     * Window provider
    /******************************************************************************************************************/

    // Create window container
    window_container_ = std::make_unique<window_provider::WindowContainer>();

    // Create window manager
    window_manager_ = std::make_unique<window_provider::WindowManager>(*window_container_);

    // Create window adder
    window_adder_ = std::make_unique<window_provider::WindowAdder>(*window_container_);

    // Create window eraser
    window_eraser_ = std::make_unique<window_provider::WindowEraser>(*window_container_);

    return true; // Setup successful
}

bool WindowService::PreUpdate()
{
    // Begin frame update
    BeginFrame();

    // Call base class PreUpdate
    if (!mono_service::Service::PreUpdate())
        return false;

    return true;
}

bool WindowService::Update()
{
    // Call base class Update
    if (!mono_service::Service::Update())
        return false;

    bool result = false;
    
    while (!GetExecutableCommandQueue().IsEmpty())
    {
        // Dequeue command list
        std::unique_ptr<mono_service::ServiceCommandList> command_list
            = GetExecutableCommandQueue().DequeueCommandList();

        // Execute all commands in the command list
        for (const auto& command : command_list->GetCommands())
        {
            // Execute command
            result = command->Execute(GetAPI());
            if (!result)
                return false; // Stop update on failure
        }
    }

    return true; // Update successful
}

bool WindowService::PostUpdate()
{
    // Call base class PostUpdate
    if (!mono_service::Service::PostUpdate())
        return false;

    // End frame update
    EndFrame();

    return true;
}

std::unique_ptr<mono_service::ServiceCommandList> WindowService::CreateCommandList()
{
    return std::make_unique<WindowServiceCommandList>();
}

std::unique_ptr<mono_service::ServiceView> WindowService::CreateView()
{
    return std::make_unique<WindowServiceView>(GetAPI());
}

window_provider::WindowAdder& WindowService::GetWindowAdder()
{
    assert(IsSetup() && "Window service is not set up");
    return *window_adder_;
}

window_provider::WindowEraser& WindowService::GetWindowEraser()
{
    assert(IsSetup() && "Window service is not set up");
    return *window_eraser_;
}

window_provider::Window& WindowService::GetWindow(
    const window_provider::WindowHandle& window_handle)
{
    assert(IsSetup() && "Window service is not set up");

    // The pointer to the window
    window_provider::Window* window_ptr = nullptr;

    window_provider::WindowManager::GetInstance().WithLock([&](window_provider::WindowManager& manager)
    {
        // Get the window by handle
        window_provider::Window& window = manager.GetWindow(window_handle);

        // Store the pointer
        window_ptr = &window;
    });

    return *window_ptr; // Return the window
}

const window_provider::Window& WindowService::GetWindow(
    const window_provider::WindowHandle& window_handle) const
{
    assert(IsSetup() && "Window service is not set up");

    // Lock for shared access
    std::shared_lock<std::shared_mutex> lock = LockShared();
    
    // The pointer to the window
    const window_provider::Window* window_ptr = nullptr;

    window_provider::WindowManager::GetInstance().WithLock([&](const window_provider::WindowManager& manager)
    {
        // Get the window by handle
        const window_provider::Window& window = manager.GetWindow(window_handle);

        // Store the pointer
        window_ptr = &window;
    });

    return *window_ptr; // Return the window
}

} // namespace mono_window_service