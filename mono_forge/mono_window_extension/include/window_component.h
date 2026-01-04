#pragma once

#include "ecs/include/component.h"
#include "window_provider/include/window_handle.h"
#include "window_provider/include/win32_window.h"
#include "mono_service/include/service.h"
#include "mono_window_extension/include/dll_config.h"

namespace mono_window_extension
{

// The handle class for the component
class MONO_WINDOW_EXT_DLL WindowComponentHandle : public ecs::ComponentHandle<WindowComponentHandle> {};

// Default window style
constexpr DWORD DEFAULT_WINDOW_STYLE = WS_OVERLAPPEDWINDOW;

// Default window titlew
constexpr const wchar_t* DEFAULT_WINDOW_TITLE = L"MonoWindow";

// Default window class name
constexpr const wchar_t* DEFAULT_WINDOW_CLASS_NAME = L"MonoWindow";

// Default window width
constexpr uint32_t DEFAULT_WINDOW_WIDTH = 800;

// Default window height
constexpr uint32_t DEFAULT_WINDOW_HEIGHT = 600;

// The component class
class MONO_WINDOW_EXT_DLL WindowComponent :
    public ecs::Component
{
public:
    WindowComponent(std::unique_ptr<mono_service::ServiceProxy> window_service_proxy);
    virtual ~WindowComponent() override;

    // The type for window procedure function
    using WindowProcType = LRESULT(CALLBACK*)(HWND, UINT, WPARAM, LPARAM);

    class SetupParam : //REFLECTABLE_CLASS_BEGIN// 
        public ecs::Component::SetupParam
    {
    public:
        SetupParam() :
            window_style(DEFAULT_WINDOW_STYLE),
            title(DEFAULT_WINDOW_TITLE),
            class_name(DEFAULT_WINDOW_CLASS_NAME),
            window_proc(nullptr),
            width(DEFAULT_WINDOW_WIDTH),
            height(DEFAULT_WINDOW_HEIGHT),
            show_state(window_provider::win32::WindowShowState::SHOW)
        {
        }

        virtual ~SetupParam() override = default;

        // The window style
        DWORD window_style;

        // Window title
        std::wstring title;

        // Window class name
        std::wstring class_name;

        // Window procedure function
        WindowProcType window_proc;

        // Window width
        uint32_t width;

        // Window height
        uint32_t height;

        window_provider::win32::WindowShowState show_state;

    }; //REFLECTABLE_CLASS_END//

    virtual bool Setup(ecs::Component::SetupParam& param) override;
    virtual ecs::ComponentID GetID() const override;

    // Show the window
    void Show();

    // Hide the window
    void Hide();

    // Resize the window
    void Resize(uint32_t width, uint32_t height);

    // Maximize the window
    void Maximize();

    // Minimize the window
    void Minimize();

    // Restore the window
    void Restore();

    // Fullscreen the window
    void Fullscreen();

    // Check if the window is created
    bool IsCreated() const { return window_handle_.IsValid(); }

    // Get the native Win32 window handle
    HWND GetHwnd() const;

    // Get the window width
    uint32_t GetWidth() const;

    // Get the window height
    uint32_t GetHeight() const;

    // Get the window client width
    uint32_t GetClientWidth() const;

    // Get the window client height
    uint32_t GetClientHeight() const;

    // Get the window position X
    uint32_t GetPositionX() const;

    // Get the window position Y
    uint32_t GetPositionY() const;

private:
    // The service proxy to the window service
    std::unique_ptr<mono_service::ServiceProxy> window_service_proxy_ = nullptr;

    // The window handle associated with this component
    window_provider::WindowHandle window_handle_ = window_provider::WindowHandle();
};

} // namespace mono_window_extension