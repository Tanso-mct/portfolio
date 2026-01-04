#pragma once
#include "mono_service/include/service.h"

#include "window_provider/include/window_manager.h"

#include "mono_window_service/include/dll_config.h"
#include "mono_service/include/service_registry.h"

namespace mono_window_service
{

// The window_service API
// It provides access to window_service internals for commands
class MONO_WINDOW_SERVICE_DLL WindowServiceAPI :
    public mono_service::ServiceAPI
{
public:
    WindowServiceAPI() = default;
    virtual ~WindowServiceAPI() = default;

    // Get window adder
    virtual window_provider::WindowAdder& GetWindowAdder() = 0;

    // Get window eraser
    virtual window_provider::WindowEraser& GetWindowEraser() = 0;

    // Get window by window handle
    virtual window_provider::Window& GetWindow(
        const window_provider::WindowHandle& window_handle) = 0;

    // Get const window by window handle
    virtual const window_provider::Window& GetWindow(
        const window_provider::WindowHandle& window_handle) const = 0;
};

// The number of command queue buffers for window_service
constexpr size_t SERVICE_COMMAND_QUEUE_BUFFER_COUNT = 2;

// The transform service handle type
class MONO_WINDOW_SERVICE_DLL WindowServiceHandle : public mono_service::ServiceHandle<WindowServiceHandle> {};

// The window_service class
class MONO_WINDOW_SERVICE_DLL WindowService :
    public mono_service::Service,
    private WindowServiceAPI
{
public:
    WindowService(mono_service::ServiceThreadAffinityID thread_affinity_id);
    virtual ~WindowService();

    /*******************************************************************************************************************
     * Service
    /******************************************************************************************************************/

    class SetupParam :
        public mono_service::Service::SetupParam
    {
    public:
        SetupParam() :
            mono_service::Service::SetupParam(SERVICE_COMMAND_QUEUE_BUFFER_COUNT)
        {
        }

        virtual ~SetupParam() override = default;
    };
    virtual bool Setup(mono_service::Service::SetupParam& param) override;
    virtual bool PreUpdate() override;
    virtual bool Update() override;
    virtual bool PostUpdate() override;
    virtual std::unique_ptr<mono_service::ServiceCommandList> CreateCommandList() override;
    virtual std::unique_ptr<mono_service::ServiceView> CreateView() override;

protected:
    /*******************************************************************************************************************
     * WindowService API
    /******************************************************************************************************************/

    WindowServiceAPI& GetAPI() { return *this; }
    const WindowServiceAPI& GetAPI() const { return *this; }

    virtual window_provider::WindowAdder& GetWindowAdder() override;
    virtual window_provider::WindowEraser& GetWindowEraser() override;
    virtual window_provider::Window& GetWindow(
        const window_provider::WindowHandle& window_handle) override;
    virtual const window_provider::Window& GetWindow(
        const window_provider::WindowHandle& window_handle) const override;

private:
    /*******************************************************************************************************************
     * Window provider
    /******************************************************************************************************************/

    std::unique_ptr<window_provider::WindowContainer> window_container_ = nullptr;
    std::unique_ptr<window_provider::WindowManager> window_manager_ = nullptr;
    std::unique_ptr<window_provider::WindowAdder> window_adder_ = nullptr;
    std::unique_ptr<window_provider::WindowEraser> window_eraser_ = nullptr;
};

} // namespace mono_window_service