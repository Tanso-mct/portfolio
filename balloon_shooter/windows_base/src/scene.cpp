#include "windows_base/src/pch.h"
#include "windows_base/include/scene.h"

#include "windows_base/include/type_cast.h"

#include "windows_base/include/asset_registry.h"
#include "windows_base/include/asset_factory_registry.h"
#include "windows_base/include/file_loader_registry.h"

#include "windows_base/include/container_impl.h"
#include "windows_base/include/system.h"

void wb::SceneContext::SetEntityContainer(std::unique_ptr<IEntityContainer> entityCont)
{
    entityContainer_ = std::move(entityCont);
}

wb::IEntityContainer &wb::SceneContext::GetEntityContainer()
{
    if (entityContainer_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Entity container is not set."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return *entityContainer_;
}

void wb::SceneContext::SetComponentContainer(std::unique_ptr<IComponentContainer> componentCont)
{
    componentContainer_ = std::move(componentCont);
}

wb::IComponentContainer &wb::SceneContext::GetComponentContainer()
{
    if (componentContainer_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Component container is not set."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return *componentContainer_;
}

void wb::SceneContext::SetSystemContainer(std::unique_ptr<ISystemContainer> systemCont)
{
    systemContainer_ = std::move(systemCont);
}

wb::ISystemContainer &wb::SceneContext::GetSystemContainer()
{
    if (systemContainer_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"System container is not set."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return *systemContainer_;
}

void wb::SceneContext::SetEntityIDView(std::unique_ptr<IEntityIDView> entityIDView)
{
    entityIDView_ = std::move(entityIDView);
}

wb::IEntityIDView &wb::SceneContext::GetEntityIDView()
{
    if (entityIDView_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Entity ID view is not set."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return *entityIDView_;
}

void wb::SceneFacade::SetContext(std::unique_ptr<IContext> context)
{
    sceneContext_ = wb::UniqueAs<ISceneContext>(context);
    if (sceneContext_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Context is not of type ISceneContext."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }
}

bool wb::SceneFacade::CheckIsReady() const
{
    if (sceneContext_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "Scene facade is not ready: ",
                "Scene context is not set."
            }
        );
        wb::ConsoleLogWrn(err);
        return false;
    }

    if (entitiesFactory_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "Scene facade is not ready: ",
                "Entities factory is not set."
            }
        );

        wb::ConsoleLogWrn(err);
        return false;
    }

    if (entityIDViewFactory_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "Scene facade is not ready: ",
                "Entity ID view factory is not set."
            }
        );

        wb::ConsoleLogWrn(err);
        return false;
    }

    if (systemsFactory_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "Scene facade is not ready: ",
                "Systems factory is not set."
            }
        );

        wb::ConsoleLogWrn(err);
        return false;
    }

    if (assetGroup_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "Scene facade is not ready: ",
                "Asset group is not set."
            }
        );

        wb::ConsoleLogWrn(err);
        return false;
    }

    if (systemScheduler_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "Scene facade is not ready: ",
                "System scheduler is not set."
            }
        );

        wb::ConsoleLogWrn(err);
        return false;
    }

    return true;
}

void wb::SceneFacade::SetEntitiesFactory(std::unique_ptr<IEntitiesFactory> entityFactory)
{
    entitiesFactory_ = std::move(entityFactory);
}

void wb::SceneFacade::SetEntityIDViewFactory(std::unique_ptr<IEntityIDViewFactory> entityIDViewFactory)
{
    entityIDViewFactory_ = std::move(entityIDViewFactory);
}

void wb::SceneFacade::SetSystemsFactory(std::unique_ptr<ISystemsFactory> systemsFactory)
{
    systemsFactory_ = std::move(systemsFactory);
}

void wb::SceneFacade::SetAssetGroup(std::unique_ptr<IAssetGroup> assetGroup)
{
    assetGroup_ = std::move(assetGroup);
}

void wb::SceneFacade::SetSystemScheduler(std::unique_ptr<ISystemScheduler> systemScheduler)
{
    systemScheduler_ = std::move(systemScheduler);
}

void wb::SceneFacade::Load(IAssetContainer &assetCont)
{
    if (CheckIsReady() == false)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Scene facade is not ready for loading."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    /*******************************************************************************************************************
     * Load assets
    /******************************************************************************************************************/

    // The file datas which is already loaded
    std::unordered_map<std::string, std::unique_ptr<IFileData>> fileDatas;

    // Store all asset factory IDs
    std::vector<size_t> assetFactoryIDs;

    // Load file datas and create assets
    for (const size_t &assetID : assetGroup_->GetAssetIDs())
    {
        if (assetCont.Has(assetID))
        {
            // Skip if the asset is already loaded
            continue;
        }

        std::string_view filePath = gAssetRegistry.GetFilePath(assetID);
        if (fileDatas.find(filePath.data()) == fileDatas.end())
        {
            // Get the file loader
            const size_t &fileLoaderID = gAssetRegistry.GetFileLoaderID(assetID);
            IFileLoader &fileLoader = gFileLoaderRegistry.GetLoader(fileLoaderID);

            // Load the file data
            std::unique_ptr<IFileData> fileData = fileLoader.Load(filePath);

            // Store the file data
            fileDatas[filePath.data()] = std::move(fileData);
        }

        // Get the asset factory
        const size_t &assetFactoryID = gAssetRegistry.GetFactoryID(assetID);
        IAssetFactory &assetFactory = gAssetFactoryRegistry.GetFactory(assetFactoryID);

        // Create the asset using the factory and the loaded file data
        std::unique_ptr<IAsset> asset = assetFactory.Create(*fileDatas[filePath.data()]);

        // Add the asset to the asset container
        assetCont.Set(assetID, std::move(asset));

        // Store the asset factory ID
        if (std::find(assetFactoryIDs.begin(), assetFactoryIDs.end(), assetFactoryID) == assetFactoryIDs.end())
        {
            assetFactoryIDs.push_back(assetFactoryID);
        }
    }

    for (const size_t &assetFactoryID : assetFactoryIDs)
    {
        // Get the asset factory
        IAssetFactory &assetFactory = gAssetFactoryRegistry.GetFactory(assetFactoryID);
        
        assetFactory.CreateAfter();
    }

    // Clean up the file datas
    fileDatas.clear();

    /*******************************************************************************************************************
     * Create systems
    /******************************************************************************************************************/

    sceneContext_->SetSystemContainer(systemsFactory_->Create(assetCont));

    /*******************************************************************************************************************
     * Create entities
    /******************************************************************************************************************/

    // Create containers
    sceneContext_->SetComponentContainer(std::make_unique<ComponentContainer>());
    sceneContext_->SetEntityContainer(std::make_unique<EntityContainer>());

    // Create entity ID view 
    sceneContext_->SetEntityIDView(entityIDViewFactory_->Create());

    // Create entities using the factory
    entitiesFactory_->Create
    (
        assetCont,
        sceneContext_->GetEntityContainer(),
        sceneContext_->GetComponentContainer(),
        sceneContext_->GetEntityIDView()
    );
}

wb::SceneState wb::SceneFacade::Update
(
    ContainerStorage &contStorage, const double &deltaTime, 
    const size_t &belongWindowID, size_t &nextSceneID
){
    wb::SystemArgument args
    {
        sceneContext_->GetEntityContainer(),
        sceneContext_->GetComponentContainer(),
        sceneContext_->GetEntityIDView(),
        sceneContext_->GetSystemContainer(),
        contStorage,
        deltaTime, belongWindowID, nextSceneID
    };

    systemScheduler_->Execute(sceneContext_->GetSystemContainer(), args);

    return args.state_;
}

void wb::SceneFacade::Release(IAssetContainer &assetCont)
{
    if (CheckIsReady() == false)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Scene facade is not ready for releasing."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    // Clear the system container
    sceneContext_->SetSystemContainer(nullptr);

    // Clear assets
    for (const size_t& assetID : assetGroup_->GetAssetIDs())
    {
        assetCont.Release(assetID);
    }

    // Clear the entity container
    sceneContext_->SetEntityContainer(nullptr);

    // Clear the component container
    sceneContext_->SetComponentContainer(nullptr);

    // Clear the entity ID view
    sceneContext_->SetEntityIDView(nullptr);
}

void wb::SceneUpdator::SetBelongWindowID(const size_t &belongWindowID)
{
    belongWindowID_ = belongWindowID;
    isBelongWindowIDSet_ = true;
}

void wb::SceneUpdator::SetInitialSceneID(const size_t &initialSceneID)
{
    currentSceneID_ = initialSceneID;
    nextSceneID_ = initialSceneID;
    isInitialSceneIDSet_ = true;
}

bool wb::SceneUpdator::CheckIsReady() const
{
    if (isBelongWindowIDSet_ == false)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "Scene updator is not ready: ",
                "Belong window ID is not set."
            }
        );

        wb::ConsoleLogWrn(err);
        return false;
    }

    if (isInitialSceneIDSet_ == false)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "Scene updator is not ready: ",
                "Initial scene ID is not set."
            }
        );

        wb::ConsoleLogWrn(err);
        return false;
    }

    return true;
}

bool wb::SceneUpdator::NeedToLoad() const
{
    return currentSceneState_ == SceneState::NeedToLoad;
}

bool wb::SceneUpdator::NeedToExit() const
{
    return currentSceneState_ == SceneState::NeedToExit;
}

bool wb::SceneUpdator::IsLoading() const
{
    return isLoading_;
}

bool wb::SceneUpdator::IsSwitching() const
{
    return currentSceneState_ == SceneState::Switching;
}

bool wb::SceneUpdator::IsFinishedLoading()
{
    if (!asyncloadFuture_.valid())
    {
        return true;
    }

    if (asyncloadFuture_.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
        isLoading_ = false;
        return true;
    }

    return false;
}

bool wb::SceneUpdator::IsFinishedReleasing()
{
    if (!asyncReleaseFuture_.valid())
    {
        return true;
    }

    if (asyncReleaseFuture_.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
        return true;
    }

    return false;
}

void wb::SceneUpdator::MoveToNextScene()
{
    currentSceneID_ = nextSceneID_;
}

void wb::SceneUpdator::AsyncLoadNextScene(IAssetContainer &assetCont, ISceneContainer &sceneCont)
{
    wb::ISceneFacade *nextScene = sceneCont.PtrGet(nextSceneID_);
    if (nextScene == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Next scene is not exist with ID: ", std::to_string(nextSceneID_)}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    if (nextScene->CheckIsReady() == false)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Next scene is not ready for loading with ID: ", std::to_string(nextSceneID_)}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    if (!IsFinishedLoading())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Scene updator is already async loading a scene."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    isLoading_ = true;
    asyncloadFuture_ = std::async
    (
        std::launch::async,
        &wb::ISceneFacade::Load, nextScene, std::ref(assetCont)
    );
}

void wb::SceneUpdator::SyncLoadNextScene(IAssetContainer &assetCont, ISceneContainer &sceneCont)
{
    wb::ISceneFacade *nextScene = sceneCont.PtrGet(nextSceneID_);
    if (nextScene == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Next scene is not exist with ID: ", std::to_string(nextSceneID_)}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    if (nextScene->CheckIsReady() == false)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Next scene is not ready for loading with ID: ", std::to_string(nextSceneID_)}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    nextScene->Load(assetCont);
    isLoading_ = false;
}

void wb::SceneUpdator::UpdateCurrentScene(ContainerStorage &contStorage)
{
    // Create the first update time if it is not set
    if (isFirstUpdate_)
    {
        lastUpdateTime_ = std::chrono::high_resolution_clock::now();
        isFirstUpdate_ = false;
    }

    // Get the current update time
    currentUpdateTime_ = std::chrono::high_resolution_clock::now();

    // Calculate the delta time
    std::chrono::duration<double> deltaTime = currentUpdateTime_ - lastUpdateTime_;
    lastUpdateTime_ = currentUpdateTime_;

    wb::ISceneContainer &sceneCont = contStorage.GetContainer<ISceneContainer>();
    wb::ISceneFacade *currentScene = sceneCont.PtrGet(currentSceneID_);
    if (currentScene == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Current scene is not exist with ID: ", std::to_string(currentSceneID_)}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    // Update the scene state
    currentSceneState_ = currentScene->Update(contStorage, deltaTime.count(), belongWindowID_, nextSceneID_);
}

void wb::SceneUpdator::AsyncReleaseCurrentScene(IAssetContainer &assetCont, ISceneContainer &sceneCont)
{
    wb::ISceneFacade *currentScene = sceneCont.PtrGet(currentSceneID_);
    if (currentScene == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Current scene is not exist with ID: ", std::to_string(currentSceneID_)}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    if (!IsFinishedReleasing())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Scene updator is already async releasing a scene."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    asyncReleaseFuture_ = std::async
    (
        std::launch::async,
        &wb::ISceneFacade::Release, currentScene, std::ref(assetCont)
    );
}

void wb::SceneUpdator::SyncReleaseCurrentScene(IAssetContainer &assetCont, ISceneContainer &sceneCont)
{
    wb::ISceneFacade *currentScene = sceneCont.PtrGet(currentSceneID_);
    if (currentScene == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Current scene is not exist with ID: ", std::to_string(currentSceneID_)}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    currentScene->Release(assetCont);
}