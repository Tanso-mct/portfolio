#pragma once
#include "windows_base/windows_base.h"

#include "wbp_primitive/include/primitive.h"
#pragma comment(lib, "wbp_primitive.lib")

#include <DirectXMath.h>

namespace wbp_collision
{
    class IColliderShapeAsset : public wb::IAsset
    {
    public:
        virtual ~IColliderShapeAsset() = default;

        virtual std::vector<DirectX::XMFLOAT3> &GetVertices() = 0;

        virtual std::vector<wbp_primitive::PrimitiveAABB> &GetAABBs() = 0;
        virtual wbp_primitive::PrimitiveAABB &GetTotalAABB() = 0;
    };

} // namespace wbp_collision