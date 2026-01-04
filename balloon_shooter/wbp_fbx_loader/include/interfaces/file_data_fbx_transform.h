#pragma once
#include "windows_base/windows_base.h"

#include <DirectXMath.h>

namespace wbp_fbx_loader
{
    struct FBXTransform
    {
        DirectX::XMFLOAT3 position_;
        DirectX::XMFLOAT3 rotation_;
        DirectX::XMFLOAT3 scale_;
    };

    class IFBXTransformFileData : public wb::IFileData
    {
    public:
        virtual ~IFBXTransformFileData() = default;

        virtual void AddTransform(const FBXTransform &transform) = 0;
        virtual const std::vector<FBXTransform> &GetTransforms() const = 0;
    };

} // namespace wbp_fbx_loader