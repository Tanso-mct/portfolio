#pragma once
#include "wbp_primitive/include/dll_config.h"

#include "wbp_primitive/include/primitive.h"

#include <vector>

namespace wbp_primitive
{
    WBP_PRIMITIVE_API PrimitiveAABB CreateAABBFromAABBs
    (
        const std::vector<PrimitiveAABB> &aabbs, 
        const DirectX::XMMATRIX &convertMat = DirectX::XMMatrixIdentity()
    );

    WBP_PRIMITIVE_API PrimitiveAABB CreateAABBFromVec
    (
        const DirectX::XMFLOAT3 &vec, const DirectX::XMFLOAT3 &origin
    );

    WBP_PRIMITIVE_API PrimitiveAABB CreateAABBFromAABBMovement
    (
        const PrimitiveAABB &aabb,
        const DirectX::XMMATRIX &beforeMat, const DirectX::XMMATRIX &afterMat
    );

    WBP_PRIMITIVE_API bool IntersectAABBs
    (
        const PrimitiveAABB &aabb1, const DirectX::XMMATRIX &aabb1ConvertMat,
        const PrimitiveAABB &aabb2, const DirectX::XMMATRIX &aabb2ConvertMat
    );

    WBP_PRIMITIVE_API DirectX::XMFLOAT3 GetCollidedFaceNormal
    (
        const PrimitiveAABB &aabb1, const DirectX::XMMATRIX &aabb1ConvertMat,
        const PrimitiveAABB &aabb2, const DirectX::XMMATRIX &aabb2ConvertMat,
        const DirectX::XMFLOAT3 &movement
    );

    WBP_PRIMITIVE_API bool IntersectRayAABB
    (
        const PrimitiveRay& ray, const PrimitiveAABB& aabb, const DirectX::XMMATRIX &aabbConvertMat, 
        float* tmin = nullptr, float* tmax = nullptr
    );

    WBP_PRIMITIVE_API PrimitiveAABB CreateAABBFromRay
    (
        const PrimitiveRay &ray, const DirectX::XMMATRIX &convertMat = DirectX::XMMatrixIdentity()
    );

} // namespace wbp_primitive