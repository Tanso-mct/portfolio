#include "mono_asset/src/pch.h"
#include "mono_asset/include/locator.h"

#pragma comment(lib, "riaecs.lib")

using namespace DirectX;

#include "mono_file/mono_file.h"
#pragma comment(lib, "mono_file.lib")

std::unique_ptr<riaecs::IAssetStagingArea> mono_asset::AssetFactoryLocator::Prepare() const
{
    return std::make_unique<AssetStagingAreaLocator>();
}

std::unique_ptr<riaecs::IAsset> mono_asset::AssetFactoryLocator::Create(
    const riaecs::IFileData& fileData, riaecs::IAssetStagingArea& stagingArea) const
{
    // Get FBX transform-only file data
    const mono_file::FileDataFBXTransformOnly* fbxData
        = dynamic_cast<const mono_file::FileDataFBXTransformOnly*>(&fileData);
    assert(fbxData); // Should be valid

    // Create locator asset
    std::unique_ptr<AssetLocator> locatorAsset = std::make_unique<AssetLocator>();

    // If there is at least one transform, use the first one to set the locator's transform
    const auto& transforms = fbxData->GetTransforms();
    assert(!transforms.empty()); // There should be at least one transform

    for (const auto& transform : transforms)
    {
        AssetLocator::LocatedTransform locatedTransform;
        locatedTransform.translation_ = transform.translation_;
        locatedTransform.rotation_ = transform.rotation_;
        locatedTransform.scale_ = transform.scale_;
        locatorAsset->AddTransform(locatedTransform);
    }

    return locatorAsset; // Return the created locator asset
}

void mono_asset::AssetFactoryLocator::Commit(riaecs::IAssetStagingArea& stagingArea) const
{
}

MONO_ASSET_API riaecs::AssetFactoryRegistrar<mono_asset::AssetFactoryLocator> mono_asset::AssetFactoryLocatorID;