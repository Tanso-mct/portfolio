#include "mono_asset_service/src/pch.h"
#include "mono_asset_service/include/asset_service.h"

#include "utility_header/logger.h"

#include "mono_asset_service/include/asset_service_command_list.h"
#include "mono_asset_service/include/asset_service_view.h"

namespace mono_asset_service
{

AssetService::AssetService(mono_service::ServiceThreadAffinityID thread_affinity_id) :
    mono_service::Service(thread_affinity_id)
{
}

AssetService::~AssetService()
{
    /*******************************************************************************************************************
     * Asset loader cleanup
    /******************************************************************************************************************/

    loader_unregistrar_.reset();
    loader_registrar_.reset();
    loader_manager_.reset();
    loader_registry_.reset();

    asset_unregistrar_.reset();
    asset_registrar_.reset();
    asset_manager_.reset();
    asset_registry_.reset();

    handle_id_generator_.reset();
    loader_id_generator_.reset();
}

bool AssetService::Setup(mono_service::Service::SetupParam& param)
{
    // Call base class Setup
    if (!mono_service::Service::Setup(param))
        return false;

    /*******************************************************************************************************************
     * Asset loader
    /******************************************************************************************************************/

    // Create singleton asset loader id generator
    loader_id_generator_ = std::make_unique<asset_loader::AssetLoaderIDGenerator>();

    // Create singleton asset loader registry
    loader_registry_ = std::make_unique<asset_loader::AssetLoaderRegistry>();

    // Create asset loader manager
    loader_manager_ = std::make_unique<asset_loader::AssetLoaderManager>(*loader_registry_);

    // Create asset loader registrar
    loader_registrar_ = std::make_unique<asset_loader::AssetLoaderRegistrar>(*loader_registry_);

    // Create asset loader unregistrar
    loader_unregistrar_ = std::make_unique<asset_loader::AssetLoaderUnregistrar>(*loader_registry_);

    // Create singleton asset handle id generator
    handle_id_generator_ = std::make_unique<asset_loader::AssetHandleIDGenerator>();

    // Create asset registry
    asset_registry_ = std::make_unique<asset_loader::AssetRegistry>();

    // Create singleton asset manager
    asset_manager_ = std::make_unique<asset_loader::AssetManager>(*asset_registry_);

    // Create asset registrar
    asset_registrar_ = std::make_unique<asset_loader::AssetRegistrar>(*asset_registry_);

    // Create asset unregistrar
    asset_unregistrar_ = std::make_unique<asset_loader::AssetUnregistrar>(*asset_registry_);

    return true; // Setup successful
}

bool AssetService::PreUpdate()
{
    // Call base class PreUpdate
    if (!mono_service::Service::PreUpdate())
        return false;

    return true;
}

bool AssetService::Update()
{
    // Begin frame update
    BeginFrame();

    // Call base class Update
    if (!mono_service::Service::Update())
        return false;

    bool result = false;

    // Execute all command lists in the executable command queue
    while (!GetExecutableCommandQueue().IsEmpty())
    {
        // Dequeue command list
        std::unique_ptr<mono_service::ServiceCommandList> command_list
            = GetExecutableCommandQueue().DequeueCommandList();

        // Execute all commands in the command list
        for (const auto& command : command_list->GetCommands())
        {
            // Execute command
            result = command->Execute(GetAPI());
            if (!result)
                return false; // Stop update on failure
        }
    }

    // The map of asset sources grouped by their loader ID
    std::unordered_map
    <asset_loader::AssetLoaderID, std::vector<std::unique_ptr<asset_loader::AssetSource>>> sources_by_loader;

    // Group asset sources by their loader ID
    for (std::unique_ptr<asset_loader::AssetSource>& source : asset_sources_)
    {
        // Get the loader ID from the asset source description
        asset_loader::AssetLoaderID loader_id = source->description->GetLoaderID();

        // Add the asset source to the corresponding loader ID group
        sources_by_loader[loader_id].emplace_back(std::move(source));
    }

    // Clear asset sources to prepare for next frame
    asset_sources_.clear();

    if (!sources_by_loader.empty())
    {
        // Launch asynchronous task to load assets
        std::future<bool> load_future 
            = std::async(std::launch::async, [sources_by_loader = std::move(sources_by_loader), this]() mutable
        {
            bool result = false;

            // Load assets for each loader ID group
            for (auto& [loader_id, sources] : sources_by_loader)
            {
                asset_loader::AssetLoaderManager::GetInstance().WithLock([&](asset_loader::AssetLoaderManager& manager)
                {
                    // Get the asset loader
                    asset_loader::AssetLoader& loader = manager.GetLoader(loader_id);

                    // Prepare staging areas for all asset sources
                    std::unique_ptr<asset_loader::LoadingStagingArea> staging_area = loader.Prepare();

                    // Load each asset source using the loader
                    for (std::unique_ptr<asset_loader::AssetSource>& source : sources)
                    {
                        // Load the asset using the loader
                        std::unique_ptr<asset_loader::Asset> asset
                            = loader.Load(*source->source_data, *staging_area);
                        if (asset == nullptr)
                            return; // If loading failed, return

                        // Add to in-progress loaded assets
                        in_progress_loaded_assets_.emplace_back(
                            LoadingAssetInfo{source->description->GetHandleID(), std::move(asset)});
                    }

                    // Commit the changes
                    result = loader.Commit(*staging_area);
                    if (!result)
                        return; // If committing failed, return

                });

                if (!result)
                    return false; // Stop update on failure
            }

            // Store source datas that are still loading
            for (auto& [loader_id, sources] : sources_by_loader)
                for (auto& source : sources)
                    loading_asset_sources_[source->description->GetHandleID()] = std::move(source);

            return true; // Loading successful
        });

        // Store the future for tracking
        loading_futures_.emplace_back(std::move(load_future));
    }

    // Iterate through loading futures and remove completed ones
    std::vector<size_t> completed_indices;
    for (size_t i = 0; i < loading_futures_.size(); ++i)
    {
        std::future<bool>& future = loading_futures_[i];
        if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
        {
            // Future is ready, get the result
            result = future.get();
            if (!result)
                return false; // Stop update on failure

            // Mark this index for removal
            completed_indices.push_back(i);
        }
    }

    if (loading_futures_.size() == completed_indices.size() && !completed_indices.empty())
    {
        // All futures are completed, clear the vector
        loading_futures_.clear();

        // Register loaded assets
        for (LoadingAssetInfo& info : in_progress_loaded_assets_)
            asset_registrar_->RegisterAsset(info.handle_id, std::move(info.asset));

        // Clear in-progress loaded assets
        in_progress_loaded_assets_.clear();
    }

    // End frame update
    EndFrame();

    return true; // Update successful
}

bool AssetService::PostUpdate()
{
    // Call base class PostUpdate
    if (!mono_service::Service::PostUpdate())
        return false;

    return true;
}

std::unique_ptr<mono_service::ServiceCommandList> AssetService::CreateCommandList()
{
    return std::make_unique<AssetServiceCommandList>();
}

std::unique_ptr<mono_service::ServiceView> AssetService::CreateView()
{
    return std::make_unique<AssetServiceView>(GetAPI());
}

asset_loader::AssetLoaderRegistrar& AssetService::GetLoaderRegistrar()
{
    assert(IsSetup() && "AssetService is not set up.");
    return *loader_registrar_;
}

asset_loader::AssetLoaderUnregistrar& AssetService::GetLoaderUnregistrar()
{
    assert(IsSetup() && "AssetService is not set up.");
    return *loader_unregistrar_;
}

asset_loader::AssetRegistrar& AssetService::GetAssetRegistrar()
{
    assert(IsSetup() && "AssetService is not set up.");
    return *asset_registrar_;
}

asset_loader::AssetUnregistrar& AssetService::GetAssetUnregistrar()
{
    assert(IsSetup() && "AssetService is not set up.");
    return *asset_unregistrar_;
}

asset_loader::AssetSources& AssetService::GetAssetSources()
{
    assert(IsSetup() && "AssetService is not set up.");
    return asset_sources_;
}

std::unordered_map<
    asset_loader::AssetHandleID, std::unique_ptr<asset_loader::AssetSource>>& AssetService::GetLoadingAssetSources()
{
    assert(IsSetup() && "AssetService is not set up.");
    return loading_asset_sources_;
}

const asset_loader::Asset& AssetService::GetAsset(asset_loader::AssetHandleID id) const
{
    assert(IsSetup() && "AssetService is not set up.");

    // Pointer to the asset to return
    asset_loader::Asset* asset_ptr = nullptr;

    asset_manager_->WithLock([&](asset_loader::AssetManager& mgr)
    {
        // Get the asset associated with the given ID
        asset_ptr = &mgr.GetAsset(id);
    });

    return *asset_ptr;
}

bool AssetService::IsAssetLoaded(asset_loader::AssetHandleID id) const
{
    assert(IsSetup() && "AssetService is not set up.");

    bool is_loaded = false;

    asset_manager_->WithLock([&](asset_loader::AssetManager& asset_manager)
    {
        // Check if the asset associated with the given ID is loaded
        is_loaded = asset_manager.Contains(id);
    });

    return is_loaded;
}

std::vector<asset_loader::AssetHandleID> AssetService::GetLoadedAssetIDs() const
{
    assert(IsSetup() && "AssetService is not set up.");

    std::vector<asset_loader::AssetHandleID> loaded_ids;

    asset_manager_->WithLock([&](asset_loader::AssetManager& asset_manager)
    {
        // Get the list of loaded asset handle IDs
        loaded_ids = asset_manager.GetRegisteredIDs();
    });

    return loaded_ids;
}

std::unordered_map<std::string, asset_loader::AssetHandleID>& AssetService::GetLoadedAssetNameToHandleIDMap()
{
    assert(IsSetup() && "AssetService is not set up.");
    return loaded_asset_name_to_handle_id_map_;
}

const std::unordered_map<std::string, asset_loader::AssetHandleID>& AssetService::GetLoadedAssetNameToHandleIDMap() const
{
    assert(IsSetup() && "AssetService is not set up.");
    return loaded_asset_name_to_handle_id_map_;
}

} // namespace mono_asset_service