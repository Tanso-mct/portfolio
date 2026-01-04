#pragma once
#include "windows_base/include/dll_config.h"

#include "windows_base/include/interfaces/monitor.h"
#include "windows_base/include/interfaces/registry.h"

#include "windows_base/include/console_log.h"
#include "windows_base/include/error_handler.h"

namespace wb
{
    class WINDOWS_BASE_API MonitorFactoryRegistry : public IMonitorFactoryRegistry
    {
    private:
        std::unordered_map<size_t, std::unique_ptr<IMonitorFactory>> monitorFactories_;
        size_t maxId = 0;
        std::vector<size_t> keys_;

    public:
        MonitorFactoryRegistry() = default;
        virtual ~MonitorFactoryRegistry() = default;

        MonitorFactoryRegistry(const MonitorFactoryRegistry &) = delete;
        MonitorFactoryRegistry &operator=(const MonitorFactoryRegistry &) = delete;

        /***************************************************************************************************************
         * IMonitorFactoryRegistry implementation
        /**************************************************************************************************************/

        void AddFactory(size_t monitorID, std::unique_ptr<IMonitorFactory> monitorFactory) override;
        IMonitorFactory &GetFactory(size_t monitorID) override;

        size_t GetMaxID() const override;
        const std::vector<size_t> &GetKeys() const override;
    };

    extern WINDOWS_BASE_API MonitorFactoryRegistry gMonitorFactoryRegistry;

    class WINDOWS_BASE_API MonitorFactoryRegistrar
    {
    public:
        MonitorFactoryRegistrar(size_t monitorID, std::unique_ptr<IMonitorFactory> monitorFactory);
    };

} // namespace wb

#define WB_REGISTER_MONITOR_FACTORY(MONITOR_FACTORY, ID) \
    static wb::MonitorFactoryRegistrar monitorFactoryRegistrar##MONITOR_FACTORY(ID, std::make_unique<MONITOR_FACTORY>());