#include "mono_scene/src/pch.h"
#include "mono_scene/include/component_scene.h"

#include "mono_scene/include/component_scene_tag.h"

mono_scene::ComponentScene::ComponentScene()
{
}

mono_scene::ComponentScene::~ComponentScene()
{
    entitiesFactory_.reset();
    assetSourceIDs_.clear();
    systemListEditCmds_.clear();

    isLoaded_ = false;
    isReleased_ = false;
    isSystemListEdited_ = false;

    needsLoad_ = false;
    needsRelease_ = false;

    needsEditSystemList_ = false;
    targetEditCmdIndex_ = 0;
}

void mono_scene::ComponentScene::Setup(SetupParam &param)
{
    std::unique_lock lock(mutex_);

    entitiesFactory_ = std::move(param.entitiesFactory_);
    assetSourceIDs_ = std::move(param.assetSourceIDs_);
    systemListEditCmds_ = std::move(param.systemListEditCmds_);
    targetEditCmdIndex_ = param.targetEditCmdIndex_;

    needsLoad_ = param.needsLoad_;
    needsEditSystemList_ = param.needsEditSystemList_;
    clear_color_ = param.clear_color_;
}

bool mono_scene::ComponentScene::IsLoaded() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return isLoaded_;
}

bool mono_scene::ComponentScene::IsReleased() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return isReleased_;
}

bool mono_scene::ComponentScene::IsSystemListEdited() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return isSystemListEdited_;
}

bool mono_scene::ComponentScene::NeedsLoad() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return needsLoad_;
}

bool mono_scene::ComponentScene::NeedsRelease() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return needsRelease_;
}

bool mono_scene::ComponentScene::NeedsEditSystemList() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return needsEditSystemList_;
}

size_t mono_scene::ComponentScene::TargetEditCommandIndex() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return targetEditCmdIndex_;
}

MONO_SCENE_API riaecs::ComponentRegistrar
<mono_scene::ComponentScene, mono_scene::ComponentSceneMaxCount> mono_scene::ComponentSceneID;

MONO_SCENE_API void mono_scene::LoadScene
(
    riaecs::Entity sceneEntity, ComponentScene *component, 
    riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont
){
    riaecs::StagingEntityArea stagingArea;

    if (component->IsLoaded())
        riaecs::NotifyError({"Scene is already loaded."}, RIAECS_LOG_LOC);

    // Cached file data for asset sources
    std::unordered_map<std::string_view, std::unique_ptr<riaecs::IFileData>>& fileDatas = component->GetFileDatas();

    // Load all file data from asset sources
    for (size_t assetSourceID : component->AssetSourceIDsRO()())
    {
        riaecs::ROObject<riaecs::AssetSource> assetSource = riaecs::gAssetSourceRegistry->Get(assetSourceID);

        std::string_view filePath = assetSource().GetFilePath();
        if (fileDatas.find(filePath) == fileDatas.end())
        {
            riaecs::ROObject<riaecs::IFileLoader> fileLoader = riaecs::gFileLoaderRegistry->Get(assetSource().GetFileLoaderID());
            fileDatas[filePath] = fileLoader().Load(filePath);
        }
    }

    // Get all asset factory IDs from asset sources
    std::unordered_set<size_t> assetFactoryIDs;
    for (size_t assetSourceID : component->AssetSourceIDsRO()())
    {
        riaecs::ROObject<riaecs::AssetSource> assetSource = riaecs::gAssetSourceRegistry->Get(assetSourceID);
        if (assetFactoryIDs.find(assetSource().GetAssetFactoryID()) == assetFactoryIDs.end())
            assetFactoryIDs.insert(assetSource().GetAssetFactoryID());
    }

    // Create all stating areas for assets
    std::unordered_map<size_t, std::unique_ptr<riaecs::IAssetStagingArea>> stagingAreas;
    for (size_t assetFactoryID : assetFactoryIDs)
    {
        riaecs::ROObject<riaecs::IAssetFactory> assetFactory = riaecs::gAssetFactoryRegistry->Get(assetFactoryID);
        stagingAreas[assetFactoryID] = assetFactory().Prepare();
    }

    // Create all assets
    for (size_t assetSourceID : component->AssetSourceIDsRO()())
    {
        riaecs::ROObject<riaecs::AssetSource> assetSource = riaecs::gAssetSourceRegistry->Get(assetSourceID);
        std::string_view filePath = assetSource().GetFilePath();
        
        if (fileDatas.find(filePath) == fileDatas.end())
            riaecs::NotifyError({"File data not found for path: " + std::string(filePath)}, RIAECS_LOG_LOC);

        riaecs::ROObject<riaecs::IAssetFactory> assetFactory = riaecs::gAssetFactoryRegistry->Get(assetSource().GetAssetFactoryID());
        size_t generation = assetCont.GetGeneration(assetSourceID);
        riaecs::ID assetID(assetSourceID, generation);

        if (assetCont.Contains(assetID))
        {
            // Asset already exists, Add reference count
            riaecs::ROObject<riaecs::IAsset> existingAsset = assetCont.Get(assetID);
            existingAsset().AddReferenceCount();
        }
        else
        {
            // Create new asset
            std::unique_ptr<riaecs::IAsset> asset 
            = assetFactory().Create(*fileDatas[filePath], *stagingAreas[assetSource().GetAssetFactoryID()]);

            // Set asset in the asset container
            assetCont.Set(assetID, std::move(asset));
        }
    }

    // Commit all staging areas
    for (const auto &pair : stagingAreas)
    {
        riaecs::ROObject<riaecs::IAssetFactory> assetFactory = riaecs::gAssetFactoryRegistry->Get(pair.first);
        assetFactory().Commit(*pair.second);
    }

    // Create entities using the entities factory
    if (component->EntitiesFactoryRO()() != nullptr)
        stagingArea = component->EntitiesFactoryRO()()->CreateEntities(sceneEntity, ecsWorld, assetCont);

    // Set the staging entity area
    component->StagingEntityAreaRW()() = std::move(stagingArea);
    
    // Mark the scene as loaded
    component->IsLoadedRW()() = true;
    component->NeedsLoadRW()() = false;

    // Mark the scene as not released
    component->IsReleasedRW()() = false;
}

MONO_SCENE_API void mono_scene::ReleaseScene
(
    const riaecs::Entity &sceneEntity, ComponentScene *component, 
    riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont
){
    if (component->IsReleased())
    {
        // Mark the scene as released
        component->IsReleasedRW()() = true;
        component->NeedsReleaseRW()() = false;

        // Mark the scene as not loaded
        component->IsLoadedRW()() = false;

        return;
    }

    // Store entities to be destroyed
    std::vector<riaecs::Entity> entitiesToDestroy;
    for (const riaecs::Entity &entity : ecsWorld.View(mono_scene::ComponentSceneTagID())())
    {
        mono_scene::ComponentSceneTag *tag 
        = riaecs::GetComponent<mono_scene::ComponentSceneTag>(ecsWorld, entity, mono_scene::ComponentSceneTagID());

        if (tag->GetSceneEntity() == sceneEntity)
            entitiesToDestroy.push_back(entity);
    }

    // Destroy all entities created by this scene
    for (const riaecs::Entity &entity : entitiesToDestroy)
        ecsWorld.DestroyEntity(entity);

    // Clear all asset sources
    for (size_t assetSourceID : component->AssetSourceIDsRO()())
    {
        // Get asset ID
        riaecs::ID assetID(assetSourceID, assetCont.GetGeneration(assetSourceID));

        // Get reference count
        size_t referenceCount = assetCont.Get(assetID)().GetReferenceCount();

        // Check reference count
        if (referenceCount >= 1)
        {
            // Just minus reference count
            assetCont.Get(assetID)().MinusReferenceCount();
            continue;
        }
        else
        {
            // Release asset
            assetCont.Release(assetID);
        }
    }

    // Mark the scene as released
    component->IsReleasedRW()() = true;
    component->NeedsReleaseRW()() = false;

    // Mark the scene as not loaded
    component->IsLoadedRW()() = false;
}

MONO_SCENE_API void mono_scene::AddSystemListEditCommand(ComponentScene *component, riaecs::ISystemLoopCommandQueue &systemLoopCmdQueue)
{
    size_t targetIndex = component->TargetEditCommandIndex();
    if (targetIndex >= component->SystemListEditCommandsRO()().size())
        riaecs::NotifyError({"Invalid target edit command index."}, RIAECS_LOG_LOC);

    systemLoopCmdQueue.Enqueue(component->SystemListEditCommandsRO()()[targetIndex]->Clone());

    // Mark the system list as edited
    component->NeedsEditSystemListRW()() = false;
}