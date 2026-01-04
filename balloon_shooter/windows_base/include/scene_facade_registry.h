#pragma once
#include "windows_base/include/dll_config.h"

#include "windows_base/include/interfaces/scene.h"
#include "windows_base/include/interfaces/registry.h"

#include "windows_base/include/scene.h"
#include "windows_base/include/system.h"
#include "windows_base/include/entity.h"

#include "windows_base/include/console_log.h"
#include "windows_base/include/error_handler.h"

namespace wb
{
    class WINDOWS_BASE_API SceneFacadeRegistry : public ISceneFacadeRegistry
    {
    private:
        std::unordered_map<size_t, std::unique_ptr<ISceneFacadeFactory>> sceneFacadeFactories_;
        size_t maxId = 0;
        std::vector<size_t> keys_;

    public:
        SceneFacadeRegistry() = default;
        virtual ~SceneFacadeRegistry() = default;

        SceneFacadeRegistry(const SceneFacadeRegistry &) = delete;
        SceneFacadeRegistry &operator=(const SceneFacadeRegistry &) = delete;

        /***************************************************************************************************************
         * ISceneFacadeRegistry implementation
        /**************************************************************************************************************/

        void AddFactory(size_t id, std::unique_ptr<ISceneFacadeFactory> factory) override;
        ISceneFacadeFactory &GetFactory(size_t id) override;

        size_t GetMaxID() const override;
        const std::vector<size_t> &GetKeys() const override;
    };

    extern WINDOWS_BASE_API SceneFacadeRegistry gSceneFacadeRegistry;

    class WINDOWS_BASE_API SceneFacadeRegistrar
    {
    public:
        SceneFacadeRegistrar(size_t id, std::unique_ptr<ISceneFacadeFactory> factory);
    };

} // namespace wb

#define WB_REGISTER_SCENE_FACADE(ID_FUC, ENTITIES_FACTORY, ASSET_GROUP, SYSTEM_SCHEDULER) \
    static wb::SceneFacadeRegistrar sceneFacadeRegistrar##ID_FUC \
    ( \
        ID_FUC(), \
        std::make_unique<wb::SceneFacadeFactory \
        < \
            wb::SceneContext, \
            ENTITIES_FACTORY, \
            wb::EntityIDViewFactory<wb::EntityIDView>, \
            wb::SystemsFactory, \
            ASSET_GROUP, \
            SYSTEM_SCHEDULER \
        >>() \
    );