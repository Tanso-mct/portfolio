#pragma once
#include "windows_base/include/dll_config.h"

#include "windows_base/include/interfaces/monitor.h"
#include "windows_base/include/interfaces/registry.h"

#include "windows_base/include/console_log.h"
#include "windows_base/include/error_handler.h"

namespace wb
{
    class WINDOWS_BASE_API MonitorRegistry : public IMonitorRegistry
    {
    private:
        std::unordered_map<size_t, size_t> factoryIDs_;
        size_t maxId = 0;
        std::vector<size_t> keys_;

    public:
        MonitorRegistry() = default;
        virtual ~MonitorRegistry() = default;

        MonitorRegistry(const MonitorRegistry &) = delete;
        MonitorRegistry &operator=(const MonitorRegistry &) = delete;

        /***************************************************************************************************************
         * IMonitorRegistry implementation
        /**************************************************************************************************************/

        void Add(size_t id, size_t factoryID) override;
        const size_t &GetFactoryID(size_t id) const override;

        size_t GetMaxID() const override;
        const std::vector<size_t> &GetKeys() const override;
    };

    extern WINDOWS_BASE_API MonitorRegistry gMonitorRegistry;

    class WINDOWS_BASE_API MonitorRegistrar
    {
    public:
        MonitorRegistrar(size_t monitorID, size_t factoryID);
    };

} // namespace wb

#define WB_REGISTER_MONITOR(ID_FUNC, FACTORY_ID) \
    static wb::MonitorRegistrar monitorRegistrar##ID_FUNC(ID_FUNC(), FACTORY_ID);