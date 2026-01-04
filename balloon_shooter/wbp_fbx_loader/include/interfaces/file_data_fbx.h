#pragma once
#include "windows_base/windows_base.h"

#include <DirectXMath.h>

namespace wbp_fbx_loader
{
    struct FBXVertex
    {
        DirectX::XMFLOAT3 position_;
        DirectX::XMFLOAT3 normal_;
        DirectX::XMFLOAT4 color_;
        DirectX::XMFLOAT2 uv_;
        unsigned int boneIndices_[4] = { 0 };
        float boneWeights_[4] = { 0.0f };
    };

    struct FBXMesh
    {
        std::string name_;
        std::vector<FBXVertex> vertices_;
        std::vector<unsigned int> indices_;
        std::string materialName_;
    };

    class IFBXFileData : public wb::IFileData
    {
    public:
        virtual ~IFBXFileData() = default;

        virtual std::vector<FBXMesh> &GetMeshes() = 0;

    };

} // namespace wbp_fbx_loader