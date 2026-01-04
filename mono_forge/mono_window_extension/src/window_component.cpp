#include "mono_window_extension/src/pch.h"
#include "mono_window_extension/include/window_component.h"

#include "mono_window_service/include/window_service_command_list.h"
#include "mono_window_service/include/window_service_view.h"

namespace mono_window_extension
{

WindowComponent::WindowComponent(std::unique_ptr<mono_service::ServiceProxy> window_service_proxy) :
    window_service_proxy_(std::move(window_service_proxy))
{
}

WindowComponent::~WindowComponent()
{
    // Create window service command list
    std::unique_ptr<mono_service::ServiceCommandList> service_command_list
        = window_service_proxy_->CreateCommandList();
    mono_window_service::WindowServiceCommandList* window_command_list
        = dynamic_cast<mono_window_service::WindowServiceCommandList*>(service_command_list.get());
    assert(window_command_list != nullptr && "Failed to create WindowServiceCommandList");

    // Destroy window by window service
    window_command_list->DestroyWindowByService(window_handle_);

    // Submit command list to window service
    window_service_proxy_->SubmitCommandList(std::move(service_command_list));
}

bool WindowComponent::Setup(ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    WindowComponent::SetupParam* window_component_param
        = dynamic_cast<WindowComponent::SetupParam*>(&param);
    assert(window_component_param != nullptr && "Invalid setup param type for WindowComponent");

    // Create window service command list
    std::unique_ptr<mono_service::ServiceCommandList> service_command_list
        = window_service_proxy_->CreateCommandList();
    mono_window_service::WindowServiceCommandList* window_command_list
        = dynamic_cast<mono_window_service::WindowServiceCommandList*>(service_command_list.get());
    assert(window_command_list != nullptr && "Failed to create WindowServiceCommandList");

    {
        // Create window create description
        std::unique_ptr<window_provider::Win32Window::CreateDesc> create_desc
            = std::make_unique<window_provider::Win32Window::CreateDesc>();
        create_desc->wc = { 
            sizeof(create_desc->wc), CS_CLASSDC, window_component_param->window_proc, 0L, 0L,
            ::GetModuleHandleW(nullptr), nullptr, nullptr, nullptr, nullptr,
            nullptr, nullptr };
        create_desc->window_style = window_component_param->window_style;
        create_desc->title = window_component_param->title;
        create_desc->width = window_component_param->width;
        create_desc->height = window_component_param->height;
        create_desc->class_name = window_component_param->class_name;
        create_desc->show_state = window_component_param->show_state;

        // Create window by window service
        window_command_list->CreateWindowByService(window_handle_, std::move(create_desc));
    }

    // Submit command list to window service
    window_service_proxy_->SubmitCommandList(std::move(service_command_list));

    return true; // Success
}

ecs::ComponentID WindowComponent::GetID() const
{
    return WindowComponentHandle::ID();
}

void WindowComponent::Show()
{
    assert(window_handle_.IsValid() && "Window handle is not valid");

    // Create window service command list
    std::unique_ptr<mono_service::ServiceCommandList> service_command_list
        = window_service_proxy_->CreateCommandList();
    mono_window_service::WindowServiceCommandList* window_command_list
        = dynamic_cast<mono_window_service::WindowServiceCommandList*>(service_command_list.get());
    assert(window_command_list != nullptr && "Failed to create WindowServiceCommandList");

    // Show window by window service
    window_command_list->ShowWindowByService(window_handle_);

    // Submit command list to window service
    window_service_proxy_->SubmitCommandList(std::move(service_command_list));
}

void WindowComponent::Hide()
{
    assert(window_handle_.IsValid() && "Window handle is not valid");

    // Create window service command list
    std::unique_ptr<mono_service::ServiceCommandList> service_command_list
        = window_service_proxy_->CreateCommandList();
    mono_window_service::WindowServiceCommandList* window_command_list
        = dynamic_cast<mono_window_service::WindowServiceCommandList*>(service_command_list.get());
    assert(window_command_list != nullptr && "Failed to create WindowServiceCommandList");

    // Hide window by window service
    window_command_list->HideWindowByService(window_handle_);

    // Submit command list to window service
    window_service_proxy_->SubmitCommandList(std::move(service_command_list));
}

void WindowComponent::Resize(uint32_t width, uint32_t height)
{
    assert(window_handle_.IsValid() && "Window handle is not valid");

    // Create window service command list
    std::unique_ptr<mono_service::ServiceCommandList> service_command_list
        = window_service_proxy_->CreateCommandList();
    mono_window_service::WindowServiceCommandList* window_command_list
        = dynamic_cast<mono_window_service::WindowServiceCommandList*>(service_command_list.get());
    assert(window_command_list != nullptr && "Failed to create WindowServiceCommandList");

    // Resize window by window service
    window_command_list->ResizeWindowByService(window_handle_, width, height);

    // Submit command list to window service
    window_service_proxy_->SubmitCommandList(std::move(service_command_list));
}

void WindowComponent::Maximize()
{
    assert(window_handle_.IsValid() && "Window handle is not valid");
    
    // Create window service command list
    std::unique_ptr<mono_service::ServiceCommandList> service_command_list
        = window_service_proxy_->CreateCommandList();
    mono_window_service::WindowServiceCommandList* window_command_list
        = dynamic_cast<mono_window_service::WindowServiceCommandList*>(service_command_list.get());
    assert(window_command_list != nullptr && "Failed to create WindowServiceCommandList");

    // Maximize window by window service
    window_command_list->MaximizeWindowByService(window_handle_);

    // Submit command list to window service
    window_service_proxy_->SubmitCommandList(std::move(service_command_list));
}

void WindowComponent::Minimize()
{
    assert(window_handle_.IsValid() && "Window handle is not valid");
    
    // Create window service command list
    std::unique_ptr<mono_service::ServiceCommandList> service_command_list
        = window_service_proxy_->CreateCommandList();
    mono_window_service::WindowServiceCommandList* window_command_list
        = dynamic_cast<mono_window_service::WindowServiceCommandList*>(service_command_list.get());
    assert(window_command_list != nullptr && "Failed to create WindowServiceCommandList");

    // Minimize window by window service
    window_command_list->MinimizeWindowByService(window_handle_);

    // Submit command list to window service
    window_service_proxy_->SubmitCommandList(std::move(service_command_list));
}

void WindowComponent::Restore()
{
    assert(window_handle_.IsValid() && "Window handle is not valid");
    
    // Create window service command list
    std::unique_ptr<mono_service::ServiceCommandList> service_command_list
        = window_service_proxy_->CreateCommandList();
    mono_window_service::WindowServiceCommandList* window_command_list
        = dynamic_cast<mono_window_service::WindowServiceCommandList*>(service_command_list.get());
    assert(window_command_list != nullptr && "Failed to create WindowServiceCommandList");

    // Restore window by window service
    window_command_list->RestoreWindowByService(window_handle_);

    // Submit command list to window service
    window_service_proxy_->SubmitCommandList(std::move(service_command_list));
}

void WindowComponent::Fullscreen()
{
    assert(window_handle_.IsValid() && "Window handle is not valid");
    
    // Create window service command list
    std::unique_ptr<mono_service::ServiceCommandList> service_command_list
        = window_service_proxy_->CreateCommandList();
    mono_window_service::WindowServiceCommandList* window_command_list
        = dynamic_cast<mono_window_service::WindowServiceCommandList*>(service_command_list.get());
    assert(window_command_list != nullptr && "Failed to create WindowServiceCommandList");

    // Fullscreen window by window service
    window_command_list->FullscreenWindowByService(window_handle_);

    // Submit command list to window service
    window_service_proxy_->SubmitCommandList(std::move(service_command_list));
}

HWND WindowComponent::GetHwnd() const
{
    assert(window_handle_.IsValid() && "Window handle is not valid");
    
    // Create window service view
    std::unique_ptr<mono_service::ServiceView> service_view
        = window_service_proxy_->CreateView();
    mono_window_service::WindowServiceView* window_view
        = dynamic_cast<mono_window_service::WindowServiceView*>(service_view.get());
    assert(window_view != nullptr && "Failed to create WindowServiceView");

    // Get HWND by window handle
    return window_view->GetWindowHwnd(window_handle_);
}

uint32_t WindowComponent::GetWidth() const
{
    assert(window_handle_.IsValid() && "Window handle is not valid");
    
    // Create window service view
    std::unique_ptr<mono_service::ServiceView> service_view
        = window_service_proxy_->CreateView();
    mono_window_service::WindowServiceView* window_view
        = dynamic_cast<mono_window_service::WindowServiceView*>(service_view.get());
    assert(window_view != nullptr && "Failed to create WindowServiceView");

    // Get window width by window handle
    return window_view->GetWindowWidth(window_handle_);
}

uint32_t WindowComponent::GetHeight() const
{
    assert(window_handle_.IsValid() && "Window handle is not valid");
    
    // Create window service view
    std::unique_ptr<mono_service::ServiceView> service_view
        = window_service_proxy_->CreateView();
    mono_window_service::WindowServiceView* window_view
        = dynamic_cast<mono_window_service::WindowServiceView*>(service_view.get());
    assert(window_view != nullptr && "Failed to create WindowServiceView");

    // Get window height by window handle
    return window_view->GetWindowHeight(window_handle_);
}

uint32_t WindowComponent::GetClientWidth() const
{
    assert(window_handle_.IsValid() && "Window handle is not valid");
    
    // Create window service view
    std::unique_ptr<mono_service::ServiceView> service_view
        = window_service_proxy_->CreateView();
    mono_window_service::WindowServiceView* window_view
        = dynamic_cast<mono_window_service::WindowServiceView*>(service_view.get());
    assert(window_view != nullptr && "Failed to create WindowServiceView");

    // Get window client width by window handle
    return window_view->GetWindowClientWidth(window_handle_);
}

uint32_t WindowComponent::GetClientHeight() const
{
    assert(window_handle_.IsValid() && "Window handle is not valid");
    
    // Create window service view
    std::unique_ptr<mono_service::ServiceView> service_view
        = window_service_proxy_->CreateView();
    mono_window_service::WindowServiceView* window_view
        = dynamic_cast<mono_window_service::WindowServiceView*>(service_view.get());
    assert(window_view != nullptr && "Failed to create WindowServiceView");

    // Get window client height by window handle
    return window_view->GetWindowClientHeight(window_handle_);
}

uint32_t WindowComponent::GetPositionX() const
{
    assert(window_handle_.IsValid() && "Window handle is not valid");
    
    // Create window service view
    std::unique_ptr<mono_service::ServiceView> service_view
        = window_service_proxy_->CreateView();
    mono_window_service::WindowServiceView* window_view
        = dynamic_cast<mono_window_service::WindowServiceView*>(service_view.get());
    assert(window_view != nullptr && "Failed to create WindowServiceView");

    // Get window position X by window handle
    return window_view->GetWindowPositionX(window_handle_);
}

uint32_t WindowComponent::GetPositionY() const
{
    assert(window_handle_.IsValid() && "Window handle is not valid");
    
    // Create window service view
    std::unique_ptr<mono_service::ServiceView> service_view
        = window_service_proxy_->CreateView();
    mono_window_service::WindowServiceView* window_view
        = dynamic_cast<mono_window_service::WindowServiceView*>(service_view.get());
    assert(window_view != nullptr && "Failed to create WindowServiceView");

    // Get window position Y by window handle
    return window_view->GetWindowPositionY(window_handle_);
}

} // namespace mono_window_extension