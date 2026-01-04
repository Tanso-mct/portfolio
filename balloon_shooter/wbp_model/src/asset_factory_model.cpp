#include "wbp_model/src/pch.h"
#include "wbp_model/include/asset_factory_model.h"

#include "wbp_fbx_loader/plugin.h"
#pragma comment(lib, "wbp_fbx_loader.lib")

#include "wbp_d3d12/plugin.h"
#pragma comment(lib, "wbp_d3d12.lib")

#include "wbp_model/include/asset_model.h"

const WBP_MODEL_API size_t &wbp_model::ModelAssetFactoryID()
{
    static size_t id = wb::IDFactory::CreateAssetFactoryID();
    return id;
}

std::unique_ptr<wb::IAsset> wbp_model::ModelAssetFactory::Create(wb::IFileData &fileData) const
{
    // Check if fileData is of type fbx
    wbp_fbx_loader::IFBXFileData* fbxFileData = wb::As<wbp_fbx_loader::IFBXFileData>(&fileData);
    if (!fbxFileData)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "Invalid file data type. Expected IFBXFileData."
            }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WBP_MODEL", err);
        wb::ThrowRuntimeError(err);
    }

    std::unique_ptr<wbp_model::IModelAsset> modelAsset = std::make_unique<wbp_model::ModelAsset>();

    // Get Gpu context
    wbp_d3d12::GPUContext &gpuContext = wbp_d3d12::GPUContext::GetInstance();
    if (!gpuContext.IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "GPU context is not created."
            }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WBP_MODEL", err);
        wb::ThrowRuntimeError(err);
    }

    // Process meshes from fbx file data
    for (wbp_fbx_loader::FBXMesh &mesh : fbxFileData->GetMeshes())
    {
        // Add mesh gpu data
        modelAsset->GetMeshDatas().emplace_back();
        wbp_model::MeshGPUData &meshGPUData = modelAsset->GetMeshDatas().back();

        /***************************************************************************************************************
         * Vertex Buffer
        /**************************************************************************************************************/

        UINT vertexBufferSize = sizeof(wbp_fbx_loader::FBXVertex) * mesh.vertices_.size();

        wbp_d3d12::CreateBuffer
        (
            gpuContext.GetDevice(),
            meshGPUData.vertexBuffer, vertexBufferSize
        );

        wbp_d3d12::UpdateBuffer
        (
            meshGPUData.vertexBuffer,
            mesh.vertices_.data(), vertexBufferSize
        );

        D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
        vertexBufferView.BufferLocation = meshGPUData.vertexBuffer->GetGPUVirtualAddress();
        vertexBufferView.SizeInBytes = vertexBufferSize;
        vertexBufferView.StrideInBytes = sizeof(wbp_fbx_loader::FBXVertex);
        meshGPUData.vertexBufferView = vertexBufferView;

        meshGPUData.vertexCount = static_cast<UINT>(mesh.vertices_.size());

        /***************************************************************************************************************
         * Index Buffer
        /**************************************************************************************************************/

        UINT indexBufferSize = sizeof(UINT) * mesh.indices_.size();

        wbp_d3d12::CreateBuffer
        (
            gpuContext.GetDevice(),
            meshGPUData.indexBuffer, indexBufferSize
        );

        wbp_d3d12::UpdateBuffer
        (
            meshGPUData.indexBuffer,
            mesh.indices_.data(), indexBufferSize
        );

        D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
        indexBufferView.BufferLocation = meshGPUData.indexBuffer->GetGPUVirtualAddress();
        indexBufferView.SizeInBytes = indexBufferSize;
        indexBufferView.Format = DXGI_FORMAT_R32_UINT;
        meshGPUData.indexBufferView = indexBufferView;

        meshGPUData.indexCount = static_cast<UINT>(mesh.indices_.size());
    }

    // Cast to IAsset
    std::unique_ptr<wb::IAsset> asset = wb::UniqueAs<wb::IAsset>(modelAsset);
    return asset;
}

namespace wbp_model
{
    WB_REGISTER_ASSET_FACTORY(ModelAssetFactoryID(), ModelAssetFactory)

} // namespace wbp_model