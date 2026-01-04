#include "wbp_texture/src/pch.h"
#include "wbp_texture/include/asset_factory_texture_2d.h"

#include "wbp_texture/include/asset_texture_2d.h"

#include "wbp_png_loader/plugin.h"
#pragma comment(lib, "wbp_png_loader.lib")

#include "wbp_d3d12/plugin.h"
#pragma comment(lib, "wbp_d3d12.lib")

using Microsoft::WRL::ComPtr;

const WBP_TEXTURE_API size_t &wbp_texture::Texture2DAssetFactoryID()
{
    static size_t id = wb::IDFactory::CreateAssetFactoryID();
    return id;
}

ComPtr<ID3D12GraphicsCommandList> wbp_texture::Texture2DAssetFactory::commandList_ = nullptr;
ComPtr<ID3D12CommandAllocator> wbp_texture::Texture2DAssetFactory::commandAllocator_ = nullptr;

std::unique_ptr<wb::IAsset> wbp_texture::Texture2DAssetFactory::Create(wb::IFileData &fileData) const
{
    // Check if command list is created
    if (commandList_ == nullptr)
    {
        // Get the GPU context
        wbp_d3d12::GPUContext &gpuContext = wbp_d3d12::GPUContext::GetInstance();
        if (!gpuContext.IsCreated())
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {"GPU context is not created."}
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_TEXTURE", err);
            wb::ThrowRuntimeError(err);
        }

        wbp_d3d12::CreateCommandAllocator(gpuContext.GetDevice(), commandAllocator_);
        wbp_d3d12::CreateCommandList(gpuContext.GetDevice(), commandAllocator_, commandList_);

        wbp_d3d12::ResetCommand(commandList_, commandAllocator_, nullptr);
    }

    // Check if fileData is of type PNG
    wbp_png_loader::IPNGFileData* pngFileData = wb::As<wbp_png_loader::IPNGFileData>(&fileData);
    if (!pngFileData)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "Invalid file data type. Expected IPNGFileData."
            }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WBP_TEXTURE", err);
        wb::ThrowRuntimeError(err);
    }

    std::unique_ptr<wbp_texture::ITexture2DAsset> textureAsset = std::make_unique<wbp_texture::Texture2DAsset>();
    
    // Get Gpu context
    wbp_d3d12::GPUContext &gpuContext = wbp_d3d12::GPUContext::GetInstance();

    // Create texture resource
    wbp_d3d12::CreateTexture2D
    (
        gpuContext.GetDevice(), textureAsset->GetResource(),
        pngFileData->GetMetadata().width, pngFileData->GetMetadata().height,
        pngFileData->GetMetadata().format, D3D12_RESOURCE_FLAG_NONE
    );

    // Create upload heap
    wbp_d3d12::CreateUploadHeap
    (
        gpuContext.GetDevice(),
        textureAsset->GetUploadHeap(), pngFileData->GetImage().GetPixelsSize()
    );

    // Store the information in the texture asset
    textureAsset->GetWidth() = pngFileData->GetMetadata().width;
    textureAsset->GetHeight() = pngFileData->GetMetadata().height;
    textureAsset->GetFormat() = pngFileData->GetMetadata().format;

    wbp_d3d12::AddUploadTextureToCmdList
    (
        commandList_, textureAsset->GetResource(), textureAsset->GetUploadHeap(),
        pngFileData->GetImage().GetPixels(),
        textureAsset->GetWidth(), textureAsset->GetHeight(), textureAsset->GetFormat()
    );
    
    // Cast to IAsset
    std::unique_ptr<wb::IAsset> asset = wb::UniqueAs<wb::IAsset>(textureAsset);
    return asset;
}

void wbp_texture::Texture2DAssetFactory::CreateAfter()
{
    if (commandList_.Get() == nullptr)
    {
        // No command list
        return;
    }

    wbp_d3d12::GPUContext &gpuContext = wbp_d3d12::GPUContext::GetInstance();

    wbp_d3d12::CloseCommand(commandList_);
    
    ComPtr<ID3D12Fence> fence;
    UINT64 fenceValue = 0;
    wbp_d3d12::CreateFence(gpuContext.GetDevice(), fence, fenceValue);

    HANDLE fenceEvent = nullptr;
    wbp_d3d12::CreateFenceEvent(fenceEvent);

    wbp_d3d12::WaitForGPU(gpuContext.GetCommandQueue(), fence, fenceValue, fenceEvent);

    std::vector<ID3D12CommandList*> commandLists;
    commandLists.push_back(commandList_.Get());
    wbp_d3d12::ExecuteCommand(gpuContext.GetCommandQueue(), commandLists.size(), commandLists);

    wbp_d3d12::WaitForGPU(gpuContext.GetCommandQueue(), fence, fenceValue, fenceEvent);

    // Reset command list and allocator for next use
    wbp_d3d12::ResetCommand(commandList_, commandAllocator_, nullptr);

    commandList_.Reset();
    commandAllocator_.Reset();
}

namespace wbp_texture
{
    WB_REGISTER_ASSET_FACTORY(Texture2DAssetFactoryID(), Texture2DAssetFactory);

} // namespace wbp_texture