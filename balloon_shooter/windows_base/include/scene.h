#pragma once
#include "windows_base/include/dll_config.h"

#include "windows_base/include/interfaces/asset.h"
#include "windows_base/include/interfaces/scene.h"

#include "windows_base/include/console_log.h"
#include "windows_base/include/error_handler.h"

#include <chrono>
#include <ctime>
#include <future>

namespace wb
{
    class WINDOWS_BASE_API SceneContext : public ISceneContext
    {
    private:
        std::unique_ptr<IEntityContainer> entityContainer_ = nullptr;
        std::unique_ptr<IComponentContainer> componentContainer_ = nullptr;
        std::unique_ptr<ISystemContainer> systemContainer_ = nullptr;
        std::unique_ptr<IEntityIDView> entityIDView_ = nullptr;

    public:
        SceneContext() = default;
        ~SceneContext() override = default;

        SceneContext(const SceneContext &) = delete;
        SceneContext &operator=(const SceneContext &) = delete;

        /***************************************************************************************************************
         * ISceneContext implementation
        /**************************************************************************************************************/

        void SetEntityContainer(std::unique_ptr<IEntityContainer> entityCont) override;
        IEntityContainer &GetEntityContainer() override;

        void SetComponentContainer(std::unique_ptr<IComponentContainer> componentCont) override;
        IComponentContainer &GetComponentContainer() override;

        void SetSystemContainer(std::unique_ptr<ISystemContainer> systemCont) override;
        ISystemContainer &GetSystemContainer() override;

        void SetEntityIDView(std::unique_ptr<IEntityIDView> entityIDView) override;
        IEntityIDView &GetEntityIDView() override;
    };

    class WINDOWS_BASE_API SceneFacade : public ISceneFacade
    {
    private:
        std::unique_ptr<ISceneContext> sceneContext_;
        SceneState state_ = SceneState::NeedToLoad;

        std::unique_ptr<IEntitiesFactory> entitiesFactory_;
        std::unique_ptr<IEntityIDViewFactory> entityIDViewFactory_;
        std::unique_ptr<ISystemsFactory> systemsFactory_;
        std::unique_ptr<IAssetGroup> assetGroup_;
        std::unique_ptr<ISystemScheduler> systemScheduler_;

    public:
        SceneFacade() = default;
        ~SceneFacade() override = default;

        SceneFacade(const SceneFacade &) = delete;
        SceneFacade &operator=(const SceneFacade &) = delete;

        /***************************************************************************************************************
         * IFacade implementation
        /**************************************************************************************************************/

        void SetContext(std::unique_ptr<IContext> context) override;
        bool CheckIsReady() const override;

        /***************************************************************************************************************
         * ISceneFacade implementation
        /**************************************************************************************************************/

        void SetEntitiesFactory(std::unique_ptr<IEntitiesFactory> entityFactory) override;
        void SetEntityIDViewFactory(std::unique_ptr<IEntityIDViewFactory> entityIDViewFactory) override;
        void SetSystemsFactory(std::unique_ptr<ISystemsFactory> systemsFactory) override;
        void SetAssetGroup(std::unique_ptr<IAssetGroup> assetGroup) override;
        void SetSystemScheduler(std::unique_ptr<ISystemScheduler> systemScheduler) override;

        void Load(IAssetContainer &assetCont) override;

        SceneState Update
        (
            ContainerStorage &contStorage, const double &deltaTime,
            const size_t &belongWindowID, size_t &nextSceneID
        ) override;

        void Release(IAssetContainer &assetCont) override;
    };

    template <
        typename SCENE_CONTEXT,
        typename ENTITIES_FACTORY,
        typename ENTITY_ID_VIEW_FACTORY,
        typename SYSTEMS_FACTORY,
        typename ASSET_GROUP,
        typename SYSTEM_SCHEDULER
    >
    class SceneFacadeFactory : public ISceneFacadeFactory
    {
    public:
        std::unique_ptr<ISceneFacade> Create() const override
        {
            std::unique_ptr<wb::ISceneFacade> sceneFacade = std::make_unique<wb::SceneFacade>();

            // Set the context
            sceneFacade->SetContext(std::make_unique<SCENE_CONTEXT>());

            // Set the factories and asset group
            sceneFacade->SetEntitiesFactory(std::make_unique<ENTITIES_FACTORY>());
            sceneFacade->SetEntityIDViewFactory(std::make_unique<ENTITY_ID_VIEW_FACTORY>());
            sceneFacade->SetSystemsFactory(std::make_unique<SYSTEMS_FACTORY>());
            sceneFacade->SetAssetGroup(std::make_unique<ASSET_GROUP>());
            sceneFacade->SetSystemScheduler(std::make_unique<SYSTEM_SCHEDULER>());

            // Check if the scene facade is ready
            if (!sceneFacade->CheckIsReady())
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Scene facade is not ready after creation."}
                );

                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WINDOWS_BASE", err);
                wb::ThrowRuntimeError(err);
            }

            return sceneFacade;
        }
    };

    class WINDOWS_BASE_API SceneUpdator : public ISceneUpdator
    {
    private:
        size_t belongWindowID_ = 0;
        bool isBelongWindowIDSet_ = false;

        size_t currentSceneID_ = 0;
        size_t nextSceneID_ = 0;
        bool isInitialSceneIDSet_ = false;

        SceneState currentSceneState_ = SceneState::NeedToLoad;

        bool isFirstUpdate_ = true;
        std::chrono::high_resolution_clock::time_point lastUpdateTime_;
        std::chrono::high_resolution_clock::time_point currentUpdateTime_;

        bool isLoading_ = false;
        std::future<void> asyncloadFuture_;
        std::future<void> asyncReleaseFuture_;

    public:
        SceneUpdator() = default;
        ~SceneUpdator() override = default;

        /***************************************************************************************************************
         * ISceneUpdator implementation
        /**************************************************************************************************************/

        void SetBelongWindowID(const size_t &belongWindowID) override;
        void SetInitialSceneID(const size_t &initialSceneID) override;
        bool CheckIsReady() const override;

        bool NeedToLoad() const override;
        bool NeedToExit() const override;

        bool IsLoading() const override;
        bool IsSwitching() const override;
        bool IsFinishedLoading() override;
        bool IsFinishedReleasing() override;

        void MoveToNextScene() override;

        void AsyncLoadNextScene(IAssetContainer &assetCont, ISceneContainer &sceneCont) override;
        void SyncLoadNextScene(IAssetContainer &assetCont, ISceneContainer &sceneCont) override;

        void UpdateCurrentScene(ContainerStorage &contStorage) override;

        void AsyncReleaseCurrentScene(IAssetContainer &assetCont, ISceneContainer &sceneCont) override;
        void SyncReleaseCurrentScene(IAssetContainer &assetCont, ISceneContainer &sceneCont) override;
    };
    
} // namespace wb
