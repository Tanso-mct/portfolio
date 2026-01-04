#pragma once

#include "asset_loader/include/asset_loader.h"
#include "asset_loader_test/tests/test_asset.h"

namespace asset_loader_test
{

constexpr int TEST_VALUE = 10;

// A simple asset source data for the test asset loader
class TestAssetSourceData :
    public asset_loader::AssetSourceData
{
public:
    TestAssetSourceData() = default;
    virtual ~TestAssetSourceData() override = default;

    int source_value = TEST_VALUE;
};

// A simple staging area for the test asset loader
class TestAssetStagingArea :
    public asset_loader::LoadingStagingArea
{
public:
    TestAssetStagingArea() = default;
    virtual ~TestAssetStagingArea() override = default;

    int temp_value = 0;
};

// A simple asset loader for the TestAsset class
class TestAssetLoader : 
    public asset_loader::AssetLoaderBase<TestAssetLoader>
{
public:
    TestAssetLoader() = default;
    virtual ~TestAssetLoader() override = default;

    virtual std::unique_ptr<asset_loader::LoadingStagingArea> Prepare() const override
    {
        return std::make_unique<TestAssetStagingArea>();
    }

    // Load an asset with source data
    virtual std::unique_ptr<asset_loader::Asset> Load(
        asset_loader::AssetSourceData& source_data, asset_loader::LoadingStagingArea& staging_area) const override
    {
        // Cast the source data to the correct type
        TestAssetSourceData* test_source_data = dynamic_cast<TestAssetSourceData*>(&source_data);
        if (!test_source_data)
            return nullptr; // Invalid source data type

        // Cast the staging area to the correct type
        TestAssetStagingArea* test_staging_area = dynamic_cast<TestAssetStagingArea*>(&staging_area);
        if (!test_staging_area)
            return nullptr; // Invalid staging area type

        std::unique_ptr<TestAsset> asset = std::make_unique<TestAsset>();
        asset->value = test_source_data->source_value;

        // Store some temporary data in the staging area
        test_staging_area->temp_value = asset->value;

        return asset;
    }

    // Commit any changes made during loading
    virtual bool Commit(asset_loader::LoadingStagingArea& staging_area) const override
    {
        // Cast the staging area to the correct type
        TestAssetStagingArea* test_staging_area = dynamic_cast<TestAssetStagingArea*>(&staging_area);
        if (!test_staging_area)
            return false; // Invalid staging area type

        // For this simple test loader, we just check if the temp_value is set correctly
        return test_staging_area->temp_value == TEST_VALUE;
    }
};

} // namespace asset_loader_test