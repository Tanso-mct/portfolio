#include "mono_asset_extension/src/pch.h"
#include "mono_asset_extension/include/texture_asset.h"

#include "asset_loader/include/asset_handle.h"

#include "utility_header/file_loader.h"
#include "utility_header/logger.h"
#include "mono_graphics_service/include/graphics_command_list.h"

using namespace DirectX;

namespace mono_asset_extension
{

TextureAsset::TextureAsset(std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy) :
    graphics_service_proxy_(std::move(graphics_service_proxy))
{
}

TextureAsset::~TextureAsset()
{
    // Create graphics command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = graphics_service_proxy_->CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(command_list.get());
    assert(graphics_command_list != nullptr && "Failed to create graphics command list.");

    // Destroy texture and upload buffer in graphics service
    graphics_command_list->DestroyResource(&texture_handle_);
    graphics_command_list->DestroyResource(&upload_buffer_handle_);

    // Submit command list to graphics service
    graphics_service_proxy_->SubmitCommandList(std::move(command_list));
}

bool TextureAsset::Setup(uint32_t width, uint32_t height, DXGI_FORMAT format, const void* data)
{
    // Create graphics command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = graphics_service_proxy_->CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(command_list.get());
    assert(graphics_command_list != nullptr && "Failed to create graphics command list.");

    // Create texture and upload buffer in graphics service
    graphics_command_list->CreateShaderResourceTexture2D(
        &texture_handle_, &upload_buffer_handle_, width, height, format, data);

    // Submit command list to graphics service
    graphics_service_proxy_->SubmitCommandList(std::move(command_list));

    return true; // Success
}

const render_graph::ResourceHandle* TextureAsset::GetTextureHandle() const
{
    assert(IsSetup() && "TextureAsset is not setup.");
    assert(texture_handle_.IsValid() && "Texture handle is not valid.");
    return &texture_handle_;
}

const render_graph::ResourceHandle* TextureAsset::GetUploadBufferHandle() const
{
    assert(IsSetup() && "TextureAsset is not setup.");
    assert(upload_buffer_handle_.IsValid() && "Upload buffer handle is not valid.");
    return &upload_buffer_handle_;
}

TextureAssetSourceData::TextureAssetSourceData(std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy) :
    graphics_service_proxy_(std::move(graphics_service_proxy))
{
}

void TextureAssetSourceData::SetImageData(DirectX::ScratchImage&& image_data)
{
    image_data_ = std::move(image_data);
    has_image_data_ = true;
}

void TextureAssetSourceData::SetImageMetadata(DirectX::TexMetadata&& image_metadata)
{
    image_metadata_ = std::move(image_metadata);
    has_image_metadata_ = true;
}

void TextureAssetSourceData::SetTextureInfo(const TextureInfo& texture_info)
{
    texture_info_ = texture_info;
    has_texture_info_ = true;
}

TextureAssetStagingArea::TextureAssetStagingArea()
{
}

std::unique_ptr<asset_loader::LoadingStagingArea> TextureAssetLoader::Prepare() const
{
    return std::make_unique<TextureAssetStagingArea>();
}

std::unique_ptr<asset_loader::Asset> TextureAssetLoader::Load(
    asset_loader::AssetSourceData& source_data, asset_loader::LoadingStagingArea& staging_area) const
{
    // Cast the source data to the correct type
    TextureAssetSourceData* mesh_source_data = dynamic_cast<TextureAssetSourceData*>(&source_data);
    assert(mesh_source_data != nullptr && "Invalid source data type");

    // Cast the staging area to the correct type
    TextureAssetStagingArea* mesh_staging_area = dynamic_cast<TextureAssetStagingArea*>(&staging_area);
    assert(mesh_staging_area != nullptr && "Invalid staging area type");

    // If file path is set, load file
    if (!mesh_source_data->GetFilePath().empty())
    {
        // Get the file path
        std::string_view file_path = mesh_source_data->GetFilePath();

        // Get file extension
        std::string_view file_extension = utility_header::GetFileExtension(file_path);

        if (file_extension == ".png")
        {
            ScratchImage image;
            TexMetadata metadata;

            HRESULT hr = E_FAIL;
            hr = LoadFromWICFile(
                std::wstring(file_path.begin(), file_path.end()).c_str(), WIC_FLAGS_FORCE_SRGB, &metadata, image);
            if (FAILED(hr))
            {
                utility_header::ConsoleLogErr(
                    {"Failed to load texture file: " + std::string(file_path)},
                    __FILE__, __LINE__, __FUNCTION__);
                return nullptr; // Failure
            }

            if (metadata.format != DXGI_FORMAT_R8G8B8A8_UNORM)
            {
                // Convert to R8G8B8A8_UNORM format
                ScratchImage converted;

                hr = DirectX::Convert
                (
                    image.GetImages(), image.GetImageCount(), metadata,
                    DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::TEX_FILTER_DEFAULT, 0.0f, converted
                );
                if (FAILED(hr))
                {
                    utility_header::ConsoleLogErr(
                        {"Failed to convert texture format for file: " + std::string(file_path)},
                        __FILE__, __LINE__, __FUNCTION__);
                    return nullptr; // Failure
                }
                
                // Store converted image and format
                image = std::move(converted);
                metadata.format = DXGI_FORMAT_R8G8B8A8_UNORM;
            }

            // Store image data and metadata in source data
            mesh_source_data->SetImageData(std::move(image));
            mesh_source_data->SetImageMetadata(std::move(metadata));
        }
        else
        {
            assert(false && "Unsupported texture file extension");
            return nullptr; // Unsupported file extension
        }
    }

    // Prepare texture asset
    std::unique_ptr<TextureAsset> asset = nullptr;

    if (mesh_source_data->HasImageData() && mesh_source_data->HasImageMetadata())
    {
        // Create texture asset instance
        asset = TextureAsset::CreateInstance<TextureAsset>(
            std::move(mesh_source_data->GetServiceProxy().Clone()),
            static_cast<uint32_t>(mesh_source_data->GetImageMetadata().width),
            static_cast<uint32_t>(mesh_source_data->GetImageMetadata().height),
            mesh_source_data->GetImageMetadata().format,
            mesh_source_data->GetImageData().GetPixels());

        // Get file name from file path
        std::string file_name = utility_header::GetFileNameFromPath(mesh_source_data->GetFilePath());

        // Set asset file path
        asset->SetFilePath(std::string(mesh_source_data->GetFilePath()));
    }
    else if (mesh_source_data->HasTextureInfo())
    {
        // Create empty texture asset instance
        TextureAssetSourceData::TextureInfo texture_info = mesh_source_data->GetTextureInfo();
        asset = TextureAsset::CreateInstance<TextureAsset>(
            std::move(mesh_source_data->GetServiceProxy().Clone()),
            texture_info.width, texture_info.height, texture_info.format, nullptr);
    }
    else
    {
        assert(false && "No valid image data found in texture source data.");
    }

    if (!mesh_source_data->GetName().empty())
    {
        // Use name from source data if set
        asset->SetName(mesh_source_data->GetName().data());
    }
    else
    {
        // Get file name from file path
        std::string file_name = utility_header::GetFileNameFromPath(mesh_source_data->GetFilePath());
        asset->SetName(file_name);
    }

    return asset; // Success
}

bool TextureAssetLoader::Commit(asset_loader::LoadingStagingArea& staging_area) const
{
    // Cast the staging area to the correct type
    TextureAssetStagingArea* mesh_staging_area = dynamic_cast<TextureAssetStagingArea*>(&staging_area);
    assert(mesh_staging_area != nullptr && "Invalid staging area type");

    return true; // Success
}

std::unique_ptr<asset_loader::AssetSource> TextureAssetSourceCreator::CreateAssetSource(
    std::wstring_view file_path, mono_service::ServiceProxyManager& service_proxy_manager)
{
     // Get graphics service proxy
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy = nullptr;
    service_proxy_manager.WithLock([&](mono_service::ServiceProxyManager& manager)
    {
        graphics_service_proxy = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID()).Clone();
    });

    // Create texture asset source data
    std::unique_ptr<TextureAssetSourceData> asset_source_data
        = std::make_unique<TextureAssetSourceData>(std::move(graphics_service_proxy));

    // Set file path
    asset_source_data->SetFilePath(std::string(file_path.begin(), file_path.end()));

    // Create asset description
    std::unique_ptr<asset_loader::AssetDescription> asset_description
        = std::make_unique<asset_loader::AssetDescription>(
            asset_loader::AssetHandleIDGenerator::GetInstance().Generate(),
            TextureAssetLoader::ID());

    // Create asset source
    std::unique_ptr<asset_loader::AssetSource> asset_source = std::make_unique<asset_loader::AssetSource>();
    asset_source->source_data = std::move(asset_source_data);
    asset_source->description = std::move(asset_description);

    return asset_source; // Success
}


} // namespace mono_asset_extension