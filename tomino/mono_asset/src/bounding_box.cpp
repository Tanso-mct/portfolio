#include "mono_asset/src/pch.h"
#include "mono_asset/include/bounding_box.h"

#pragma comment(lib, "riaecs.lib")
using namespace DirectX;

#include "mono_file/mono_file.h"
#pragma comment(lib, "mono_file.lib")

std::unique_ptr<riaecs::IAssetStagingArea> mono_asset::AssetFactoryBoundingBox::Prepare() const
{
    return std::make_unique<AssetStagingAreaBoundingBox>();
}

std::unique_ptr<riaecs::IAsset> mono_asset::AssetFactoryBoundingBox::Create(
    const riaecs::IFileData& fileData, riaecs::IAssetStagingArea& stagingArea) const
{
    // Get FBX MinMax-only file data
    const mono_file::FileDataFbxMinMaxOnly* fbxData
        = dynamic_cast<const mono_file::FileDataFbxMinMaxOnly*>(&fileData);
    assert(fbxData); // Should be valid

    // Create bounding box asset
    std::unique_ptr<AssetBoundingBox> boundingBoxAsset = std::make_unique<AssetBoundingBox>();

    // Set min and max from FBX data
    const auto& minMax = fbxData->GetMinMax();
    boundingBoxAsset->SetMin(minMax.min_);
    boundingBoxAsset->SetMax(minMax.max_);

    return boundingBoxAsset; // Return the created bounding box asset
}

void mono_asset::AssetFactoryBoundingBox::Commit(riaecs::IAssetStagingArea &stagingArea) const
{
}

MONO_ASSET_API riaecs::AssetFactoryRegistrar<mono_asset::AssetFactoryBoundingBox> mono_asset::AssetFactoryBoundingBoxID;