#include "asset_loader_test/pch.h"

#include "asset_loader_test/tests/test_asset_loader.h"
#include "asset_loader_test/tests/test_asset_handle.h"

#include "asset_loader/include/asset_manager.h"
#include "asset_loader/include/asset_description.h"

TEST(Asset, Handle)
{
    // Create singleton asset handle id generator
    std::unique_ptr<asset_loader::AssetHandleIDGenerator> handle_id_generator 
        = std::make_unique<asset_loader::AssetHandleIDGenerator>();

    // Create two asset handles and check their IDs
    asset_loader_test::TestAssetHandle handle;
    EXPECT_EQ(handle.ID(), 0);
    asset_loader_test::TestAssetHandle handle2;
    EXPECT_EQ(handle2.ID(), 0);
}

TEST(Asset, Loader)
{
    // Create singleton asset loader id generator
    std::unique_ptr<asset_loader::AssetLoaderIDGenerator> loader_id_generator 
        = std::make_unique<asset_loader::AssetLoaderIDGenerator>();

    // Create two asset loaders and check their IDs
    asset_loader_test::TestAssetLoader loader;
    EXPECT_EQ(loader.GetID(), 0);
    asset_loader_test::TestAssetLoader loader2;
    EXPECT_EQ(loader2.GetID(), 0);

    // Create source data
    std::unique_ptr<asset_loader_test::TestAssetSourceData> source_data 
        = std::make_unique<asset_loader_test::TestAssetSourceData>();

    // Create staging area
    std::unique_ptr<asset_loader::LoadingStagingArea> staging_area 
        = std::make_unique<asset_loader_test::TestAssetStagingArea>();

    // Load asset
    std::unique_ptr<asset_loader::Asset> asset = loader.Load(*source_data, *staging_area);
    ASSERT_NE(asset, nullptr);

    // Commit changes
    bool commit_result = loader.Commit(*staging_area);
    EXPECT_TRUE(commit_result);
}

TEST(Asset, Description)
{
    // Create source data
    std::unique_ptr<asset_loader_test::TestAssetSourceData> source_data 
        = std::make_unique<asset_loader_test::TestAssetSourceData>();

    // Create asset description
    asset_loader::AssetDescription description(
        asset_loader_test::TestAssetHandle::ID(),
        asset_loader_test::TestAssetLoader().ID());

    // Check handle ID
    EXPECT_EQ(description.GetHandleID(), asset_loader_test::TestAssetHandle::ID());

    // Check loader ID
    EXPECT_EQ(description.GetLoaderID(), asset_loader_test::TestAssetLoader().ID());
}

TEST(Asset, Register)
{
    // Create singleton instance of AssetLoaderIDGenerator
    std::unique_ptr<asset_loader::AssetLoaderIDGenerator> loader_id_generator 
        = std::make_unique<asset_loader::AssetLoaderIDGenerator>();

    // Create asset loader registry
    std::unique_ptr<asset_loader::AssetLoaderRegistry> loader_registry 
        = std::make_unique<asset_loader::AssetLoaderRegistry>();

    // Create asset loader manager
    std::unique_ptr<asset_loader::AssetLoaderManager> loader_manager
        = std::make_unique<asset_loader::AssetLoaderManager>(*loader_registry);

    // Create asset loader registrar
    std::unique_ptr<asset_loader::AssetLoaderRegistrar> loader_registrar
        = std::make_unique<asset_loader::AssetLoaderRegistrar>(*loader_registry);

    // Create asset loader unregistrar
    std::unique_ptr<asset_loader::AssetLoaderUnregistrar> loader_unregistrar
        = std::make_unique<asset_loader::AssetLoaderUnregistrar>(*loader_registry);

    // Register the test asset loader
    {
        // Create the test asset loader
        std::unique_ptr<asset_loader::AssetLoader> test_loader 
            = std::make_unique<asset_loader_test::TestAssetLoader>();

        // Register the test asset loader
        loader_registrar->Register(asset_loader_test::TestAssetLoader::ID(), std::move(test_loader));
    }

    // Create singleton asset handle id generator
    std::unique_ptr<asset_loader::AssetHandleIDGenerator> handle_id_generator 
        = std::make_unique<asset_loader::AssetHandleIDGenerator>();

    // Create asset registry
    std::unique_ptr<asset_loader::AssetRegistry> registry 
        = std::make_unique<asset_loader::AssetRegistry>();

    // Create asset manager
    std::unique_ptr<asset_loader::AssetManager> manager 
        = std::make_unique<asset_loader::AssetManager>(*registry);

    // Create asset registrar
    std::unique_ptr<asset_loader::AssetRegistrar> registrar 
        = std::make_unique<asset_loader::AssetRegistrar>(*registry);

    // Create asset unregistrar
    std::unique_ptr<asset_loader::AssetUnregistrar> unregistrar 
        = std::make_unique<asset_loader::AssetUnregistrar>(*registry);

    // Create source data
    std::unique_ptr<asset_loader_test::TestAssetSourceData> source_data 
        = std::make_unique<asset_loader_test::TestAssetSourceData>();

    // Create asset description
    asset_loader::AssetDescription description(
        asset_loader_test::TestAssetHandle::ID(),
        asset_loader_test::TestAssetLoader::ID());

    // Load asset
    std::unique_ptr<asset_loader::Asset> asset = nullptr;
    asset_loader::AssetLoaderManager::GetInstance().WithLock([&](asset_loader::AssetLoaderManager& manager)
    {
        // Get the asset loader
        asset_loader::AssetLoader& loader = manager.GetLoader(description.GetLoaderID());

        // Prepare staging area
        std::unique_ptr<asset_loader::LoadingStagingArea> staging_area = loader.Prepare();

        // Load the asset using the loader
        asset = loader.Load(*source_data, *staging_area);
        if (asset == nullptr)
            return; // If loading failed, return

        // Commit the changes
        ASSERT_TRUE(loader.Commit(*staging_area));
    });

    // Register asset
    registrar->RegisterAsset(description.GetHandleID(), std::move(asset));

    // Get asset
    manager->WithLock([&](asset_loader::AssetManager& mgr) 
    {
        asset_loader::Asset& registered_asset = mgr.GetAsset(description.GetHandleID());
        asset_loader_test::TestAsset* test_asset = dynamic_cast<asset_loader_test::TestAsset*>(&registered_asset);
        ASSERT_NE(test_asset, nullptr);
    });

    // Unregister asset
    unregistrar->UnregisterAsset(description.GetHandleID());
}