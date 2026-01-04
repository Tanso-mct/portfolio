#include "mono_asset/src/pch.h"
#include "mono_asset/include/texture.h"

#include "mono_graphics_service/include/graphics_command_list.h"
#include "mono_adapter/include/service_adapter.h"

#pragma comment(lib, "riaecs.lib")

using namespace DirectX;

#include "mono_file/mono_file.h"
#pragma comment(lib, "mono_file.lib")

mono_asset::AssetTexture::AssetTexture(std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy) :
    graphics_service_proxy_(std::move(graphics_service_proxy))
{
    assert(graphics_service_proxy_ != nullptr && "Graphics service proxy is null");
}

mono_asset::AssetTexture::~AssetTexture()
{
    // Create graphics service command list to delete buffers
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = graphics_service_proxy_->CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(command_list.get());
    assert(graphics_command_list != nullptr && "Failed to cast to GraphicsCommandList");

    // Destroy texture resource
    graphics_command_list->DestroyResource(&texture_handle_);

    // Destroy upload buffer resource
    graphics_command_list->DestroyResource(&upload_buffer_handle_);

    // Submit command list to graphics service
    graphics_service_proxy_->SubmitCommandList(std::move(command_list));
}

const render_graph::ResourceHandle* mono_asset::AssetTexture::GetTextureHandle() const
{
    assert(texture_handle_.IsValid() && "Texture handle is not valid");
    return &texture_handle_;
}

render_graph::ResourceHandle* mono_asset::AssetTexture::GetTextureHandle()
{
    return &texture_handle_;
}

const render_graph::ResourceHandle* mono_asset::AssetTexture::GetUploadBufferHandle() const
{
    assert(upload_buffer_handle_.IsValid() && "Upload buffer handle is not valid");
    return &upload_buffer_handle_;
}

render_graph::ResourceHandle* mono_asset::AssetTexture::GetUploadBufferHandle()
{
    return &upload_buffer_handle_;
}

mono_asset::AssetStagingAreaTexture::AssetStagingAreaTexture(
    std::unique_ptr<mono_service::ServiceCommandList> graphics_command_list) :
    graphics_command_list_(std::move(graphics_command_list))
{
    assert(graphics_command_list_ != nullptr && "Graphics service command list is null");
}

mono_asset::AssetStagingAreaTexture::~AssetStagingAreaTexture()
{
}

mono_asset::AssetFactoryTexture::AssetFactoryTexture()
{
}

mono_asset::AssetFactoryTexture::~AssetFactoryTexture()
{
}

std::unique_ptr<riaecs::IAssetStagingArea> mono_asset::AssetFactoryTexture::Prepare() const
{
    // Get singleton graphics service adapter
    mono_adapter::GraphicsServiceAdapter& graphics_service_adapter
        = mono_adapter::GraphicsServiceAdapter::GetInstance();
        
    // Create graphics service proxy
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy 
        = graphics_service_adapter.GetGraphicsService().CreateServiceProxy();

    // Create graphics service command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = graphics_service_proxy->CreateCommandList();

    // Create and return asset staging area for texture
    return std::make_unique<AssetStagingAreaTexture>(std::move(command_list));
}

std::unique_ptr<riaecs::IAsset> mono_asset::AssetFactoryTexture::Create(
    const riaecs::IFileData &file_data, riaecs::IAssetStagingArea &staging_area) const
{
    // Get singleton graphics service adapter
    mono_adapter::GraphicsServiceAdapter& graphics_service_adapter
        = mono_adapter::GraphicsServiceAdapter::GetInstance();
        
    // Create graphics service proxy
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy 
        = graphics_service_adapter.GetGraphicsService().CreateServiceProxy();

    // Get png file data
    const mono_file::FileDataPNG* png_data = dynamic_cast<const mono_file::FileDataPNG*>(&file_data);
    if (!png_data) 
        riaecs::NotifyError({"Invalid file data type for texture asset."}, RIAECS_LOG_LOC);

    // Get asset staging area for texture
    AssetStagingAreaTexture* texture_staging_area = dynamic_cast<AssetStagingAreaTexture*>(&staging_area);
    if (!texture_staging_area)
        riaecs::NotifyError({"Invalid asset staging area type for texture asset."}, RIAECS_LOG_LOC);

    // Create texture asset
    std::unique_ptr<AssetTexture> texture_asset = std::make_unique<AssetTexture>(graphics_service_proxy->Clone());

    // Get graphics service command list
    mono_service::ServiceCommandList& graphics_command_list
        = texture_staging_area->GetGraphicsServiceCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list_ptr
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(&graphics_command_list);
    assert(graphics_command_list_ptr != nullptr && "Graphics command list is null!");

    // Create shader resource texture2D and output its handle
    graphics_command_list_ptr->CreateShaderResourceTexture2D(
        texture_asset->GetTextureHandle(), texture_asset->GetUploadBufferHandle(),
        png_data->GetMetadata().width, png_data->GetMetadata().height, png_data->GetMetadata().format,
        png_data->GetImage().GetPixels());

    return texture_asset; // Return created texture asset
}

void mono_asset::AssetFactoryTexture::Commit(riaecs::IAssetStagingArea &staging_area) const
{
    // Get singleton graphics service adapter
    mono_adapter::GraphicsServiceAdapter& graphics_service_adapter
        = mono_adapter::GraphicsServiceAdapter::GetInstance();

    // Create graphics service proxy
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy 
        = graphics_service_adapter.GetGraphicsService().CreateServiceProxy();

    // Get texture staging area
    AssetStagingAreaTexture* texture_staging_area
        = dynamic_cast<AssetStagingAreaTexture*>(&staging_area);
    assert(texture_staging_area && "Invalid staging area type for texture");

    // Submit command list to graphics service
    mono_service::ServiceProgress progress 
        = graphics_service_proxy->SubmitCommandList(std::move(texture_staging_area->TakeGraphicsServiceCommandList()));

    // Wait for the submitted commands to complete
    const int32_t progress_completion_offset = 1;
    while (graphics_service_proxy->GetProgress() <= progress + progress_completion_offset)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

MONO_ASSET_API riaecs::AssetFactoryRegistrar<mono_asset::AssetFactoryTexture> mono_asset::AssetFactoryTextureID;