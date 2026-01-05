#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include <future>

#include "class_template/thread_safer.h"

#include "asset_loader/include/asset_loader.h"
#include "asset_loader/include/asset_manager.h"
#include "asset_loader/include/asset_source.h"
#include "mono_service/include/service.h"
#include "mono_service/include/service_registry.h"

#include "mono_asset_service/include/dll_config.h"

namespace mono_asset_service
{

// The asset_service API
// It provides access to asset_service internals for commands
class MONO_ASSET_SERVICE_DLL AssetServiceAPI :
    public mono_service::ServiceAPI
{
public:
    AssetServiceAPI() = default;
    virtual ~AssetServiceAPI() = default;

    // Get the asset loader registrar
    virtual asset_loader::AssetLoaderRegistrar& GetLoaderRegistrar() = 0;

    // Get the asset loader unregistrar
    virtual asset_loader::AssetLoaderUnregistrar& GetLoaderUnregistrar() = 0;

    // Get the asset registrar
    virtual asset_loader::AssetRegistrar& GetAssetRegistrar() = 0;

    // Get the asset unregistrar
    virtual asset_loader::AssetUnregistrar& GetAssetUnregistrar() = 0;

    // Get the asset sources
    virtual asset_loader::AssetSources& GetAssetSources() = 0;

    // Get the loading asset sources
    virtual std::unordered_map<
        asset_loader::AssetHandleID, std::unique_ptr<asset_loader::AssetSource>>& GetLoadingAssetSources() = 0;

    // Get the asset associated with the given handle ID
    virtual const asset_loader::Asset& GetAsset(asset_loader::AssetHandleID id) const = 0;

    // Check if the asset associated with the given handle ID is loaded
    virtual bool IsAssetLoaded(asset_loader::AssetHandleID id) const = 0;

    // Get the list of loaded asset handle IDs
    virtual std::vector<asset_loader::AssetHandleID> GetLoadedAssetIDs() const = 0;

    // Get the asset handle ID by asset name
    virtual std::unordered_map<std::string, asset_loader::AssetHandleID>& GetLoadedAssetNameToHandleIDMap() = 0;

    // Get the asset handle ID by asset name (const version)
    virtual const std::unordered_map<std::string, asset_loader::AssetHandleID>& GetLoadedAssetNameToHandleIDMap() const = 0;
};

// The number of command queue buffers for asset_service
constexpr size_t SERVICE_COMMAND_QUEUE_BUFFER_COUNT = 2;

// The asset service handle type
class MONO_ASSET_SERVICE_DLL AssetServiceHandle : public mono_service::ServiceHandle<AssetServiceHandle> {};

// The asset_service class
class MONO_ASSET_SERVICE_DLL AssetService :
    public mono_service::Service,
    private AssetServiceAPI
{
public:
    AssetService(mono_service::ServiceThreadAffinityID thread_affinity_id);
    virtual ~AssetService();

    /*******************************************************************************************************************
     * Service
    /******************************************************************************************************************/

    class SetupParam :
        public mono_service::Service::SetupParam
    {
    public:
        SetupParam() :
            mono_service::Service::SetupParam(SERVICE_COMMAND_QUEUE_BUFFER_COUNT)
        {
        }

        virtual ~SetupParam() override = default;
    };
    virtual bool Setup(mono_service::Service::SetupParam& param) override;
    virtual bool PreUpdate() override;
    virtual bool Update() override;
    virtual bool PostUpdate() override;
    virtual std::unique_ptr<mono_service::ServiceCommandList> CreateCommandList() override;
    virtual std::unique_ptr<mono_service::ServiceView> CreateView() override;

protected:
    /*******************************************************************************************************************
     * AssetService API
    /******************************************************************************************************************/

    AssetServiceAPI& GetAPI() { return *this; }
    const AssetServiceAPI& GetAPI() const { return *this; }

    virtual asset_loader::AssetLoaderRegistrar& GetLoaderRegistrar() override;
    virtual asset_loader::AssetLoaderUnregistrar& GetLoaderUnregistrar() override;
    virtual asset_loader::AssetRegistrar& GetAssetRegistrar() override;
    virtual asset_loader::AssetUnregistrar& GetAssetUnregistrar() override;
    virtual asset_loader::AssetSources& GetAssetSources() override;
    virtual std::unordered_map<
        asset_loader::AssetHandleID, std::unique_ptr<asset_loader::AssetSource>>& GetLoadingAssetSources() override;

    virtual const asset_loader::Asset& GetAsset(asset_loader::AssetHandleID id) const override;
    virtual bool IsAssetLoaded(asset_loader::AssetHandleID id) const override;
    virtual std::vector<asset_loader::AssetHandleID> GetLoadedAssetIDs() const override;
    virtual std::unordered_map<std::string, asset_loader::AssetHandleID>& GetLoadedAssetNameToHandleIDMap() override;
    virtual const std::unordered_map<std::string, asset_loader::AssetHandleID>& GetLoadedAssetNameToHandleIDMap() const override;

private:
    /*******************************************************************************************************************
     * Asset loader
    /******************************************************************************************************************/

    std::unique_ptr<asset_loader::AssetLoaderIDGenerator> loader_id_generator_ = nullptr;
    std::unique_ptr<asset_loader::AssetLoaderRegistry> loader_registry_ = nullptr;
    std::unique_ptr<asset_loader::AssetLoaderManager> loader_manager_ = nullptr;
    std::unique_ptr<asset_loader::AssetLoaderRegistrar> loader_registrar_ = nullptr;
    std::unique_ptr<asset_loader::AssetLoaderUnregistrar> loader_unregistrar_ = nullptr;

    std::unique_ptr<asset_loader::AssetHandleIDGenerator> handle_id_generator_ = nullptr;
    std::unique_ptr<asset_loader::AssetRegistry> asset_registry_ = nullptr;
    std::unique_ptr<asset_loader::AssetManager> asset_manager_ = nullptr;
    std::unique_ptr<asset_loader::AssetRegistrar> asset_registrar_ = nullptr;
    std::unique_ptr<asset_loader::AssetUnregistrar> asset_unregistrar_ = nullptr;

    asset_loader::AssetSources asset_sources_;

    // Loading asset sources carried over to the next frame
    std::unordered_map<
    asset_loader::AssetHandleID, std::unique_ptr<asset_loader::AssetSource>> loading_asset_sources_;

    // Futures for loading assets
    std::vector<std::future<bool>> loading_futures_;

    // In-progress loaded asset info
    struct LoadingAssetInfo
    {
        asset_loader::AssetHandleID handle_id = asset_loader::AssetHandleID();
        std::unique_ptr<asset_loader::Asset> asset = nullptr;
    };
    std::vector<LoadingAssetInfo> in_progress_loaded_assets_;

    // Map from loaded asset name to handle ID
    std::unordered_map<std::string, asset_loader::AssetHandleID> loaded_asset_name_to_handle_id_map_;
};

} // namespace mono_asset_service