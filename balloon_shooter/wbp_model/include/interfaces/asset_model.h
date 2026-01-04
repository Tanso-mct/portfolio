#pragma once
#include "windows_base/windows_base.h"

#include <d3d12.h>
#include <wrl/client.h>

namespace wbp_model
{
    struct MeshGPUData
    {
        Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
        D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
        D3D12_INDEX_BUFFER_VIEW indexBufferView;
        UINT vertexCount;
        UINT indexCount;
    };

    class IModelAsset : public wb::IAsset
    {
    public:
        virtual ~IModelAsset() = default;

        virtual std::vector<MeshGPUData> &GetMeshDatas() = 0;
    };

} // namespace wbp_model