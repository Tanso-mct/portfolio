#include "mono_asset_service_test/pch.h"

#include "mono_service/include/service_importer.h"
#include "mono_service/include/thread_affinity.h"

#include "mono_asset_service/include/asset_service.h"
#include "mono_asset_service/include/asset_service_command_list.h"
#include "mono_asset_service/include/asset_service_view.h"

#include "mono_asset_service_test/tests/test_asset_handle.h"
#include "mono_asset_service_test/tests/test_asset_loader.h"

namespace mono_transform_service_test
{

bool ImportAssetService(mono_service::ServiceRegistry& service_registry)
{
    // Create service importer
    std::unique_ptr<mono_service::ServiceImporter> service_importer 
        = std::make_unique<mono_service::ServiceImporter>(service_registry);

    // Import transform service in to registry
    std::unique_ptr<mono_asset_service::AssetService> transform_service 
        = std::make_unique<mono_asset_service::AssetService>(0);

    // Create setup parameters
    mono_asset_service::AssetService::SetupParam setup_param;

    // Import transform service
    return service_importer->Import(
        std::move(transform_service), mono_asset_service::AssetServiceHandle::ID(), setup_param);
}

} // namespace mono_transform_service_test

TEST(AssetService, Import)
{
    /*******************************************************************************************************************
     * Import asset service
    /******************************************************************************************************************/

    // Create service id generator
    std::unique_ptr<mono_service::ServiceIDGenerator> service_id_generator 
        = std::make_unique<mono_service::ServiceIDGenerator>();

    // Create service registry
    std::unique_ptr<mono_service::ServiceRegistry> service_registry 
        = std::make_unique<mono_service::ServiceRegistry>();

    // Import asset service in to registry
    bool result = mono_transform_service_test::ImportAssetService(*service_registry);
    ASSERT_TRUE(result);
}

TEST(AssetService, Execute)
{
    bool result = false;

    /*******************************************************************************************************************
     * Import asset service
    /******************************************************************************************************************/

    // Create service id generator
    std::unique_ptr<mono_service::ServiceIDGenerator> service_id_generator 
        = std::make_unique<mono_service::ServiceIDGenerator>();

    // Create service registry
    std::unique_ptr<mono_service::ServiceRegistry> service_registry 
        = std::make_unique<mono_service::ServiceRegistry>();

    // Import asset service in to registry
    result = mono_transform_service_test::ImportAssetService(*service_registry);
    ASSERT_TRUE(result);

    // Get asset service proxy from asset service
    std::unique_ptr<mono_service::ServiceProxy> asset_service_proxy = nullptr;
    service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        // Get asset service
        mono_service::Service* service 
            = &registry.Get(mono_asset_service::AssetServiceHandle::ID());

        // Create service proxy
        asset_service_proxy = service->CreateServiceProxy();
    });

    /*******************************************************************************************************************
     * Load asset
    /******************************************************************************************************************/

    {
        // Create asset service command list
        std::unique_ptr<mono_service::ServiceCommandList> command_list 
            = asset_service_proxy->CreateCommandList();
        mono_asset_service::AssetServiceCommandList& asset_command_list
            = dynamic_cast<mono_asset_service::AssetServiceCommandList&>(*command_list);

        // Create asset loader
        std::unique_ptr<asset_loader_test::TestAssetLoader> test_loader 
            = std::make_unique<asset_loader_test::TestAssetLoader>();

        // Register the test asset loader in asset service
        asset_command_list.RegisterLoader(
            asset_loader_test::TestAssetLoader::ID(), std::move(test_loader));

        // Create asset source data
        std::unique_ptr<asset_loader_test::TestAssetSourceData> source_data 
            = std::make_unique<asset_loader_test::TestAssetSourceData>();

        // Create asset description
        std::unique_ptr<asset_loader::AssetDescription> description 
            = std::make_unique<asset_loader::AssetDescription>(
                asset_loader_test::TestAssetHandle::ID(),
                asset_loader_test::TestAssetLoader::ID());

        // Create asset source
        std::unique_ptr<asset_loader::AssetSource> asset_source 
            = std::make_unique<asset_loader::AssetSource>();
        asset_source->source_data = std::move(source_data);
        asset_source->description = std::move(description);

        // Load asset in asset service
        asset_command_list.LoadAsset(std::move(asset_source));

        // Submit command list to asset service
        asset_service_proxy->SubmitCommandList(std::move(command_list));
    }

    // Wait for asset loading to complete
    bool loading_complete = false;
    while(!loading_complete)
    {
        service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
        {
            // Get asset service
            mono_service::Service& service = registry.Get(mono_asset_service::AssetServiceHandle::ID());

            // Pre-update
            result = service.PreUpdate();
            ASSERT_TRUE(result);

            // Update
            result = service.Update();
            ASSERT_TRUE(result);

            // Post-update
            result = service.PostUpdate();
            ASSERT_TRUE(result);
        });

        // Create asset service view
        std::unique_ptr<mono_service::ServiceView> service_view
            = asset_service_proxy->CreateView();
        mono_asset_service::AssetServiceView* asset_view
            = dynamic_cast<mono_asset_service::AssetServiceView*>(service_view.get());
        assert(asset_view != nullptr && "Asset service view is null!");

        // Check if asset is loaded
        loading_complete = asset_view->IsAssetLoaded(asset_loader_test::TestAssetHandle::ID());
    }

    /*******************************************************************************************************************
     * Verify asset
    /******************************************************************************************************************/

    {
        // Create asset service view
        std::unique_ptr<mono_service::ServiceView> service_view
            = asset_service_proxy->CreateView();
        mono_asset_service::AssetServiceView& asset_view
            = dynamic_cast<mono_asset_service::AssetServiceView&>(*service_view);

        // Get asset
        const asset_loader::Asset& asset
            = asset_view.GetAsset(asset_loader_test::TestAssetHandle::ID());

        // Verify asset
        asset_loader_test::TestAsset* test_asset 
            = dynamic_cast<asset_loader_test::TestAsset*>(const_cast<asset_loader::Asset*>(&asset));
        ASSERT_NE(test_asset, nullptr);
        ASSERT_EQ(test_asset->value, asset_loader_test::TEST_VALUE);
    }

    /*******************************************************************************************************************
     * Release asset
    /******************************************************************************************************************/

    {
        // Create asset service command list
        std::unique_ptr<mono_service::ServiceCommandList> command_list 
            = asset_service_proxy->CreateCommandList();
        mono_asset_service::AssetServiceCommandList& asset_command_list
            = dynamic_cast<mono_asset_service::AssetServiceCommandList&>(*command_list);

        // Release asset in asset service
        asset_command_list.ReleaseAsset(asset_loader_test::TestAssetHandle::ID());

        // Submit command list to asset service
        asset_service_proxy->SubmitCommandList(std::move(command_list));
    }

    service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        // Get asset service
        mono_service::Service& service = registry.Get(mono_asset_service::AssetServiceHandle::ID());

        // Pre-update
        result = service.PreUpdate();
        ASSERT_TRUE(result);

        // Update
        result = service.Update();
        ASSERT_TRUE(result);

        // Post-update
        result = service.PostUpdate();
        ASSERT_TRUE(result);
    });

    /*******************************************************************************************************************
     * Cleanup
    /******************************************************************************************************************/

    // Clear service registry
    service_registry.reset();
}