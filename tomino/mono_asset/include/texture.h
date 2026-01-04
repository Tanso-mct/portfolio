#pragma once
#include "mono_asset/include/dll_config.h"
#include "riaecs/riaecs.h"

#include "mono_service/include/service_registry.h"
#include "render_graph/include/resource_handle.h"

namespace mono_asset
{
    class MONO_ASSET_API AssetTexture : public riaecs::IAsset
    {
    private:
        // Service proxy for asset management
        std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy_ = nullptr;

        // Texture resource handle
        render_graph::ResourceHandle texture_handle_ = {};

        // Upload buffer resource handle
        render_graph::ResourceHandle upload_buffer_handle_ = {};

    public:
        AssetTexture(std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy);
        ~AssetTexture() override;

        // Get texture resource handle const
        const render_graph::ResourceHandle* GetTextureHandle() const;

        // Get texture resource handle
        render_graph::ResourceHandle* GetTextureHandle();

        // Get upload buffer resource handle const
        const render_graph::ResourceHandle* GetUploadBufferHandle() const;

        // Get upload buffer resource handle
        render_graph::ResourceHandle* GetUploadBufferHandle();
    };

    class MONO_ASSET_API AssetStagingAreaTexture : public riaecs::IAssetStagingArea
    {
    private:
        std::unique_ptr<mono_service::ServiceCommandList> graphics_command_list_ = nullptr;

    public:
        AssetStagingAreaTexture(std::unique_ptr<mono_service::ServiceCommandList> graphics_command_list);
        ~AssetStagingAreaTexture() override;

        // Get the graphics service command list
        mono_service::ServiceCommandList& GetGraphicsServiceCommandList()
        {
            return *graphics_command_list_;
        }

        // Take ownership of the graphics service command list
        std::unique_ptr<mono_service::ServiceCommandList> TakeGraphicsServiceCommandList()
        {
            return std::move(graphics_command_list_);
        }
    };

    class MONO_ASSET_API AssetFactoryTexture : public riaecs::IAssetFactory
    {
    public:
        AssetFactoryTexture();
        ~AssetFactoryTexture();

        std::unique_ptr<riaecs::IAssetStagingArea> Prepare() const override;
        std::unique_ptr<riaecs::IAsset> Create(
            const riaecs::IFileData &file_data, riaecs::IAssetStagingArea &staging_area) const override;
        void Commit(riaecs::IAssetStagingArea &staging_area) const override;
    };
    extern MONO_ASSET_API riaecs::AssetFactoryRegistrar<AssetFactoryTexture> AssetFactoryTextureID;

} // namespace mono_asset