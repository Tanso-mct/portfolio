#pragma once
#include "windows_base/include/dll_config.h"

#include "windows_base/include/interfaces/system.h"
#include "windows_base/include/interfaces/registry.h"

#include "windows_base/include/console_log.h"
#include "windows_base/include/error_handler.h"

namespace wb
{
    class WINDOWS_BASE_API SystemRegistry : public ISystemRegistry
    {
    private:
        std::unordered_map<size_t, std::unique_ptr<ISystemFactory>> systemFactories_;
        size_t maxId = 0;
        std::vector<size_t> keys_;

    public:
        SystemRegistry() = default;
        virtual ~SystemRegistry() = default;

        SystemRegistry(const SystemRegistry &) = delete;
        SystemRegistry &operator=(const SystemRegistry &) = delete;

        /***************************************************************************************************************
         * ISystemRegistry implementation
        /**************************************************************************************************************/

        void AddFactory(size_t systemID, std::unique_ptr<ISystemFactory> systemFactory) override;
        ISystemFactory &GetFactory(size_t systemID) override;

        size_t GetMaxID() const override;
        const std::vector<size_t> &GetKeys() const override;
    };

    extern WINDOWS_BASE_API SystemRegistry gSystemRegistry;

    template <typename SYSTEM>
    class SystemFactory : public ISystemFactory
    {
    public:
        SystemFactory() = default;
        virtual ~SystemFactory() override = default;

        std::unique_ptr<ISystem> Create(IAssetContainer& assetCont) const override
        {
            std::unique_ptr<ISystem> system = std::make_unique<SYSTEM>();
            system->Initialize(assetCont);

            return system;
        }
    };

    class WINDOWS_BASE_API SystemRegistrar
    {
    public:
        SystemRegistrar(size_t systemID, std::unique_ptr<ISystemFactory> systemFactory);
    };

} // namespace wb

#define WB_REGISTER_SYSTEM(SYSTEM, ID) \
    static wb::SystemRegistrar systemRegistrar##SYSTEM(ID, std::make_unique<wb::SystemFactory<SYSTEM>>());