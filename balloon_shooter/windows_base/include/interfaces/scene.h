#pragma once
#include "windows_base/include/dll_config.h"

#include "windows_base/include/interfaces/context.h"
#include "windows_base/include/interfaces/facade.h"
#include "windows_base/include/interfaces/container.h"
#include "windows_base/include/interfaces/factory.h"

namespace wb
{
    class IEntityIDView;

    class ISceneContext : public IContext
    {
    public:
        virtual ~ISceneContext() = default;

        virtual void SetEntityContainer(std::unique_ptr<IEntityContainer> entityCont) = 0;
        virtual IEntityContainer &GetEntityContainer() = 0;

        virtual void SetComponentContainer(std::unique_ptr<IComponentContainer> componentCont) = 0;
        virtual IComponentContainer &GetComponentContainer() = 0;

        virtual void SetSystemContainer(std::unique_ptr<ISystemContainer> systemCont) = 0;
        virtual ISystemContainer &GetSystemContainer() = 0;

        virtual void SetEntityIDView(std::unique_ptr<IEntityIDView> entityIDView) = 0;
        virtual IEntityIDView &GetEntityIDView() = 0;
    };

    class ContainerStorage;

    enum class SceneState
    {
        NeedToLoad,
        Updating,
        NeedToExit,
        Switching,
        Size,
    };

    class IAssetGroup;
    class ISystemScheduler;

    class ISceneFacade : public IFacade
    {
    public:
        virtual ~ISceneFacade() = default;

        /***************************************************************************************************************
         * Factories and Asset Group setters
        /**************************************************************************************************************/

        virtual void SetEntitiesFactory(std::unique_ptr<IEntitiesFactory> entityFactory) = 0;
        virtual void SetEntityIDViewFactory(std::unique_ptr<IEntityIDViewFactory> entityIDViewFactory) = 0;
        virtual void SetSystemsFactory(std::unique_ptr<ISystemsFactory> systemsFactory) = 0;
        virtual void SetAssetGroup(std::unique_ptr<IAssetGroup> assetGroup) = 0;
        virtual void SetSystemScheduler(std::unique_ptr<ISystemScheduler> systemScheduler) = 0;

        /***************************************************************************************************************
         * Scene management methods
        /**************************************************************************************************************/

        virtual void Load(IAssetContainer &assetCont) = 0;

        virtual SceneState Update
        (
            ContainerStorage &contStorage, const double &deltaTime,
            const size_t &belongWindowID, size_t &nextSceneID
        ) = 0;

        virtual void Release(IAssetContainer &assetCont) = 0;
    };


    class ISceneUpdator
    {
    public:
        virtual ~ISceneUpdator() = default;

        /***************************************************************************************************************
         * Setup for update
         * Sets ids for the current scene and the window it belongs to.
        /**************************************************************************************************************/

        virtual void SetBelongWindowID(const size_t &belongWindowID) = 0;
        virtual void SetInitialSceneID(const size_t &initialSceneID) = 0;
        virtual bool CheckIsReady() const = 0;

        /***************************************************************************************************************
         * Get the action is necessary
        /**************************************************************************************************************/

        virtual bool NeedToLoad() const = 0;
        virtual bool NeedToExit() const = 0;

        /***************************************************************************************************************
         * Get the action is currently in progress
        /**************************************************************************************************************/

        virtual bool IsLoading() const = 0;
        virtual bool IsSwitching() const = 0;

        virtual bool IsFinishedLoading() = 0;
        virtual bool IsFinishedReleasing() = 0;

        /***************************************************************************************************************
         * Processing for Scene
        /**************************************************************************************************************/

        virtual void MoveToNextScene() = 0;

        virtual void AsyncLoadNextScene(IAssetContainer &assetCont, ISceneContainer &sceneCont) = 0;
        virtual void SyncLoadNextScene(IAssetContainer &assetCont, ISceneContainer &sceneCont) = 0;

        virtual void UpdateCurrentScene(ContainerStorage &contStorage) = 0;

        virtual void AsyncReleaseCurrentScene(IAssetContainer &assetCont, ISceneContainer &sceneCont) = 0;
        virtual void SyncReleaseCurrentScene(IAssetContainer &assetCont, ISceneContainer &sceneCont) = 0;
    };

} // namespace wb