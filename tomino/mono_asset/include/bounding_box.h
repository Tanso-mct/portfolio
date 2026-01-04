#pragma once
#include "mono_asset/include/dll_config.h"
#include "riaecs/riaecs.h"

#include <DirectXMath.h>

namespace mono_asset
{
    class AssetBoundingBox : public riaecs::IAsset
    {
    public:
        AssetBoundingBox() = default;
        ~AssetBoundingBox() override = default;

        const DirectX::XMFLOAT3& GetMin() const { return min_; }
        void SetMin(const DirectX::XMFLOAT3& min) { min_ = min; }

        const DirectX::XMFLOAT3& GetMax() const { return max_; }
        void SetMax(const DirectX::XMFLOAT3& max) { max_ = max; }

    private:
        DirectX::XMFLOAT3 min_{0.0f, 0.0f, 0.0f};
        DirectX::XMFLOAT3 max_{0.0f, 0.0f, 0.0f};
    };

    class AssetStagingAreaBoundingBox : public riaecs::IAssetStagingArea
    {
    public:
        AssetStagingAreaBoundingBox() = default;
        ~AssetStagingAreaBoundingBox() override = default;
    };

    class MONO_ASSET_API AssetFactoryBoundingBox : public riaecs::IAssetFactory
    {
    public:
        AssetFactoryBoundingBox() = default;
        ~AssetFactoryBoundingBox() override = default;

        std::unique_ptr<riaecs::IAssetStagingArea> Prepare() const override;
        std::unique_ptr<riaecs::IAsset> Create(
            const riaecs::IFileData &fileData, riaecs::IAssetStagingArea &stagingArea) const override;
        void Commit(riaecs::IAssetStagingArea &stagingArea) const override;
    };

    extern MONO_ASSET_API riaecs::AssetFactoryRegistrar<AssetFactoryBoundingBox> AssetFactoryBoundingBoxID;
    
} // namespace mono_asset