#pragma once
#include "windows_base/include/dll_config.h"

#include "windows_base/include/interfaces/component.h"
#include "windows_base/include/interfaces/registry.h"

#include "windows_base/include/console_log.h"
#include "windows_base/include/error_handler.h"

namespace wb
{
    class WINDOWS_BASE_API ComponentRegistry : public IComponentRegistry
    {
    private:
        std::unordered_map<size_t, std::unique_ptr<IComponentFactory>> componentFactories_;
        size_t maxId = 0;
        std::vector<size_t> keys_;

    public:
        ComponentRegistry() = default;
        virtual ~ComponentRegistry() = default;

        ComponentRegistry(const ComponentRegistry &) = delete;
        ComponentRegistry &operator=(const ComponentRegistry &) = delete;

        /***************************************************************************************************************
         * IComponentRegistry implementation
        /**************************************************************************************************************/

        void AddFactory(size_t componentID, std::unique_ptr<IComponentFactory> componentFactory) override;
        IComponentFactory &GetFactory(size_t componentID) override;

        size_t GetMaxID() const override;
        const std::vector<size_t> &GetKeys() const override;
    };

    extern WINDOWS_BASE_API ComponentRegistry gComponentRegistry;

    template <typename COMPONENT>
    class ComponentFactory : public IComponentFactory
    {
    public:
        ComponentFactory() = default;
        virtual ~ComponentFactory() override = default;

        std::unique_ptr<IComponent> Create() const override
        {
            return std::make_unique<COMPONENT>();
        }
    };

    class WINDOWS_BASE_API ComponentRegistrar
    {
    public:
        ComponentRegistrar(size_t componentID, std::unique_ptr<IComponentFactory> componentFactory);
    };


} // namespace wb

#define WB_REGISTER_COMPONENT(ID, COMPONENT) \
    static wb::ComponentRegistrar componentRegistrar##COMPONENT(ID, std::make_unique<wb::ComponentFactory<COMPONENT>>());