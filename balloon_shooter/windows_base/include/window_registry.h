#pragma once
#include "windows_base/include/dll_config.h"

#include "windows_base/include/interfaces/window.h"
#include "windows_base/include/interfaces/registry.h"

#include "windows_base/include/console_log.h"
#include "windows_base/include/error_handler.h"

namespace wb
{
    class WINDOWS_BASE_API WindowRegistry : public IWindowRegistry
    {
    private:
        std::unordered_map<size_t, std::unique_ptr<IWindowFacadeFactory>> facadeFactories_;
        std::unordered_map<size_t, std::unique_ptr<IWindowEventFactory>> eventFactories_;
        size_t maxId_ = 0;
        std::vector<size_t> keys_;

    public:
        WindowRegistry() = default;
        virtual ~WindowRegistry() = default;

        WindowRegistry(const WindowRegistry &) = delete;
        WindowRegistry &operator=(const WindowRegistry &) = delete;

        /***************************************************************************************************************
         * IWindowRegistry implementation
        /**************************************************************************************************************/

        virtual void AddFactories
        (
            size_t id, 
            std::unique_ptr<IWindowFacadeFactory> facadeFactory, 
            std::unique_ptr<IWindowEventFactory> eventFactory
        ) override;
        virtual IWindowFacadeFactory &GetFacadeFactory(size_t id) override;
        virtual IWindowEventFactory &GetEventFactory(size_t id) override;

        virtual size_t GetMaxID() const override;
        virtual const std::vector<size_t> &GetKeys() const override;
    };

    extern WINDOWS_BASE_API WindowRegistry gWindowRegistry;

    class WINDOWS_BASE_API WindowRegistrar
    {
    public:
        WindowRegistrar
        (
            size_t windowID,
            std::unique_ptr<IWindowFacadeFactory> facadeFactory,
            std::unique_ptr<IWindowEventFactory> eventFactory
        );
    };

} // namespace wb

#define WB_REGISTER_WINDOW(ID_FUNC, FACADE_FACTORY, EVENT_FACTORY) \
    static wb::WindowRegistrar windowRegistrar##ID_FUNC(ID_FUNC(), std::make_unique<FACADE_FACTORY>(), std::make_unique<EVENT_FACTORY>());