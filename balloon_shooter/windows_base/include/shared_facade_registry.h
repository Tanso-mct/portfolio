#pragma once
#include "windows_base/include/dll_config.h"

#include "windows_base/include/interfaces/shared.h"
#include "windows_base/include/interfaces/registry.h"

#include "windows_base/include/console_log.h"
#include "windows_base/include/error_handler.h"

#include <memory>

namespace wb
{
    class WINDOWS_BASE_API SharedFacadeRegistry : public ISharedFacadeRegistry
    {
    private:
        std::unordered_map<size_t, std::unique_ptr<ISharedFacadeFactory>> sharedFacadeFactories_;
        size_t maxId = 0;
        std::vector<size_t> keys_;

    public:
        SharedFacadeRegistry() = default;
        virtual ~SharedFacadeRegistry() = default;

        SharedFacadeRegistry(const SharedFacadeRegistry &) = delete;
        SharedFacadeRegistry &operator=(const SharedFacadeRegistry &) = delete;

        /***************************************************************************************************************
         * ISharedFacadeRegistry implementation
        /**************************************************************************************************************/

        void AddFactory(size_t id, std::unique_ptr<ISharedFacadeFactory> factory) override;
        ISharedFacadeFactory &GetFactory(size_t id) override;

        size_t GetMaxID() const override;
        const std::vector<size_t> &GetKeys() const override;
    };

    extern WINDOWS_BASE_API SharedFacadeRegistry gSharedFacadeRegistry;

    template <typename SHARED_FACADE>
    class SharedFacadeFactory : public ISharedFacadeFactory
    {
    public:
        std::unique_ptr<ISharedFacade> Create() const override
        {
            return std::make_unique<SHARED_FACADE>();
        }
    };

    class WINDOWS_BASE_API SharedFacadeRegistrar
    {
    public:
        SharedFacadeRegistrar(size_t id, std::unique_ptr<ISharedFacadeFactory> factory);
    };

} // namespace wb

#define WB_REGISTER_SHARED_FACADE(ID_FUC, FACADE) \
    static wb::SharedFacadeRegistrar sharedFacadeRegistrar##ID_FUC(ID_FUC(), std::make_unique<wb::SharedFacadeFactory<FACADE>>());