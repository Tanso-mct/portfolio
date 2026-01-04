#pragma once

#include <memory>

#include "directxtex/DirectXTex.h"

#include "class_template/instance.h"
#include "asset_loader/include/asset.h"
#include "asset_loader/include/asset_loader.h"
#include "render_graph/include/resource_handle.h"

#include "mono_service/include/service.h"
#include "mono_asset_extension/include/dll_config.h"
#include "mono_asset_extension/include/asset_source_creator.h"

namespace mono_asset_extension
{

constexpr const char* TEXTURE_ASSET_TYPE_NAME = "Texture";

// Asset class
class MONO_ASSET_EXT_DLL TextureAsset :
    public asset_loader::Asset,
    public class_template::InstanceGuard<
        TextureAsset,
        class_template::ConstructArgList<std::unique_ptr<mono_service::ServiceProxy>>,
        class_template::SetupArgList<uint32_t, uint32_t, DXGI_FORMAT, const void*>>
{
public:
    TextureAsset(std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy);
    virtual ~TextureAsset() override;
    virtual bool Setup(uint32_t width, uint32_t height, DXGI_FORMAT format, const void* data = nullptr);
    virtual std::string_view GetTypeName() const override { return TEXTURE_ASSET_TYPE_NAME; }

    // Get texture resource handle
    const render_graph::ResourceHandle* GetTextureHandle() const;

    // Get upload buffer resource handle
    const render_graph::ResourceHandle* GetUploadBufferHandle() const;

private:
    // Service proxy for asset management
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy_ = nullptr;

    // Texture resource handle
    render_graph::ResourceHandle texture_handle_ = {};

    // Upload buffer resource handle
    render_graph::ResourceHandle upload_buffer_handle_ = {};
    
};

// Source data for the TextureAsset
class MONO_ASSET_EXT_DLL TextureAssetSourceData :
    public asset_loader::AssetSourceData
{
public:
    TextureAssetSourceData(std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy);
    virtual ~TextureAssetSourceData() override = default;

    // Get the image data
    const DirectX::ScratchImage& GetImageData() const { return image_data_; }

    // Set the image data
    void SetImageData(DirectX::ScratchImage&& image_data);

    // Check if image data is set
    bool HasImageData() const { return has_image_data_; }

    // Get the image metadata
    const DirectX::TexMetadata& GetImageMetadata() const { return image_metadata_; }

    // Set the image metadata
    void SetImageMetadata(DirectX::TexMetadata&& image_metadata);

    // Check if image metadata is set
    bool HasImageMetadata() const { return has_image_metadata_; }

    // Get the file path of the texture asset
    std::string_view GetFilePath() const { return file_path_; }

    // Set the file path of the texture asset
    void SetFilePath(std::string_view file_path) { file_path_ = std::string(file_path); }

    // Texture info structure
    struct TextureInfo
    {
        uint32_t width;
        uint32_t height;
        DXGI_FORMAT format;
    };

    // Get texture info
    TextureInfo GetTextureInfo() const { return texture_info_; }

    // Set texture info
    void SetTextureInfo(const TextureInfo& texture_info);

    // Check if texture info is set
    bool HasTextureInfo() const { return has_texture_info_; }

    // Get the service proxy for asset management
    mono_service::ServiceProxy& GetServiceProxy() { return *graphics_service_proxy_; }

private:
    // Image data
    DirectX::ScratchImage image_data_ = {};

    // Flag indicating if image data is set
    bool has_image_data_ = false;

    // Image metadata
    DirectX::TexMetadata image_metadata_ = {};

    // Flag indicating if image metadata is set
    bool has_image_metadata_ = false;

    // File path of the image
    std::string file_path_ = "";

    // Texture info
    TextureInfo texture_info_ = {};

    // Flag indicating if texture info is set
    bool has_texture_info_ = false;

    // Service proxy for asset management
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy_ = nullptr;
};

// Staging area for loading the TextureAsset
class MONO_ASSET_EXT_DLL TextureAssetStagingArea :
    public asset_loader::LoadingStagingArea
{
public:
    TextureAssetStagingArea();
    virtual ~TextureAssetStagingArea() override = default;

private:
};

// Loader class for the TextureAsset
class MONO_ASSET_EXT_DLL TextureAssetLoader :
    public asset_loader::AssetLoaderBase<TextureAssetLoader>
{
public:
    TextureAssetLoader() = default;
    virtual ~TextureAssetLoader() override = default;

    std::unique_ptr<asset_loader::LoadingStagingArea> Prepare() const override;
    std::unique_ptr<asset_loader::Asset> Load(
        asset_loader::AssetSourceData& source_data, asset_loader::LoadingStagingArea& staging_area) const override;
    bool Commit(asset_loader::LoadingStagingArea& staging_area) const override;
};

// Asset source creator for the TextureAsset
class MONO_ASSET_EXT_DLL TextureAssetSourceCreator :
    public AssetSourceCreator
{
public:
    virtual ~TextureAssetSourceCreator() override = default;
    virtual std::unique_ptr<asset_loader::AssetSource> CreateAssetSource(
        std::wstring_view file_path, mono_service::ServiceProxyManager& service_proxy_manager) override;
};

} // namespace mono_asset_extension