#pragma once
#include "mono_asset/include/dll_config.h"
#include "riaecs/riaecs.h"

#include <DirectXMath.h>

namespace mono_asset
{
    class AssetLocator : public riaecs::IAsset
    {
    public:
        AssetLocator() = default;
        ~AssetLocator() override = default;

        struct LocatedTransform
        {
            DirectX::XMFLOAT3 translation_;
            DirectX::XMFLOAT3 rotation_; // Euler angles in degrees
            DirectX::XMFLOAT3 scale_;
        };

        const std::vector<LocatedTransform>& GetTransforms() const
        {
            return transforms_;
        }

        void AddTransform(const LocatedTransform& transform)
        {
            transforms_.push_back(transform);
        }

    private:
        std::vector<LocatedTransform> transforms_;

    };

    class AssetStagingAreaLocator : public riaecs::IAssetStagingArea
    {
    public:
        AssetStagingAreaLocator() = default;
        ~AssetStagingAreaLocator() override = default;
    };

    class MONO_ASSET_API AssetFactoryLocator : public riaecs::IAssetFactory
    {
    public:
        AssetFactoryLocator() = default;
        ~AssetFactoryLocator() override = default;

        std::unique_ptr<riaecs::IAssetStagingArea> Prepare() const override;
        std::unique_ptr<riaecs::IAsset> Create(
            const riaecs::IFileData &fileData, riaecs::IAssetStagingArea &stagingArea) const override;
        void Commit(riaecs::IAssetStagingArea &stagingArea) const override;
    };
    extern MONO_ASSET_API riaecs::AssetFactoryRegistrar<AssetFactoryLocator> AssetFactoryLocatorID;

} // namespace mono_asset