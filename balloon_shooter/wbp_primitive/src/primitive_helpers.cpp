#include "wbp_primitive/src/pch.h"
#include "wbp_primitive/include/primitive_helpers.h"

using namespace DirectX;

WBP_PRIMITIVE_API wbp_primitive::PrimitiveAABB wbp_primitive::CreateAABBFromAABBs
(
    const std::vector<wbp_primitive::PrimitiveAABB> &aabbs, 
    const XMMATRIX &convertMat
){
    XMFLOAT3 min = { FLT_MAX, FLT_MAX, FLT_MAX };
    XMFLOAT3 max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

    for (const auto &aabb : aabbs)
    {
        XMFLOAT3 aabbMin = aabb.GetMin();
        XMFLOAT3 aabbMax = aabb.GetMax();

        // Transform the min and max points using the conversion matrix
        XMVECTOR transformedMin = XMVector3TransformCoord(aabb.GetMinVec(), convertMat);
        XMVECTOR transformedMax = XMVector3TransformCoord(aabb.GetMaxVec(), convertMat);

        // Update the overall min and max
        if (XMVectorGetX(transformedMin) < min.x) min.x = XMVectorGetX(transformedMin);
        if (XMVectorGetY(transformedMin) < min.y) min.y = XMVectorGetY(transformedMin);
        if (XMVectorGetZ(transformedMin) < min.z) min.z = XMVectorGetZ(transformedMin);

        if (XMVectorGetX(transformedMax) > max.x) max.x = XMVectorGetX(transformedMax);
        if (XMVectorGetY(transformedMax) > max.y) max.y = XMVectorGetY(transformedMax);
        if (XMVectorGetZ(transformedMax) > max.z) max.z = XMVectorGetZ(transformedMax);
    }

    return wbp_primitive::PrimitiveAABB(min, max);
}

WBP_PRIMITIVE_API wbp_primitive::PrimitiveAABB wbp_primitive::CreateAABBFromVec
(
    const DirectX::XMFLOAT3 &vec, const DirectX::XMFLOAT3 &origin
){
    XMVECTOR vecExtents = XMLoadFloat3(&vec) / 2.0f;
    XMVECTOR vecCenter = XMLoadFloat3(&origin) + vecExtents;

    XMFLOAT3 min, max;
    XMStoreFloat3(&min, vecCenter - vecExtents);
    XMStoreFloat3(&max, vecCenter + vecExtents);

    return wbp_primitive::PrimitiveAABB(min, max);
}

WBP_PRIMITIVE_API wbp_primitive::PrimitiveAABB wbp_primitive::CreateAABBFromAABBMovement
(
    const wbp_primitive::PrimitiveAABB &aabb,
    const DirectX::XMMATRIX &beforeMat, const DirectX::XMMATRIX &afterMat
){
    XMVECTOR beforeMin = XMVector3TransformCoord(aabb.GetMinVec(), beforeMat);
    XMVECTOR beforeMax = XMVector3TransformCoord(aabb.GetMaxVec(), beforeMat);
    wbp_primitive::PrimitiveAABB transformedAABB(beforeMin, beforeMax);

    XMVECTOR afterMin = XMVector3TransformCoord(aabb.GetMinVec(), afterMat);
    XMVECTOR afterMax = XMVector3TransformCoord(aabb.GetMaxVec(), afterMat);
    wbp_primitive::PrimitiveAABB afterAABB(afterMin, afterMax);

    return wbp_primitive::CreateAABBFromAABBs
    (
        { transformedAABB, afterAABB }, DirectX::XMMatrixIdentity()
    );
}

WBP_PRIMITIVE_API bool wbp_primitive::IntersectAABBs
(
    const wbp_primitive::PrimitiveAABB &aabb1, const DirectX::XMMATRIX &aabb1ConvertMat, 
    const wbp_primitive::PrimitiveAABB &aabb2, const DirectX::XMMATRIX &aabb2ConvertMat
){    
    // Transform the AABBs using the conversion matrices
    XMVECTOR aabb1Min = XMVector3TransformCoord(aabb1.GetMinVec(), aabb1ConvertMat);
    XMVECTOR aabb1Max = XMVector3TransformCoord(aabb1.GetMaxVec(), aabb1ConvertMat);
    XMVECTOR aabb2Min = XMVector3TransformCoord(aabb2.GetMinVec(), aabb2ConvertMat);
    XMVECTOR aabb2Max = XMVector3TransformCoord(aabb2.GetMaxVec(), aabb2ConvertMat);

    // Check for overlap in each dimension
    return 
    (   
        XMVectorGetX(aabb1Max) >= XMVectorGetX(aabb2Min) &&
        XMVectorGetY(aabb1Max) >= XMVectorGetY(aabb2Min) &&
        XMVectorGetZ(aabb1Max) >= XMVectorGetZ(aabb2Min) &&
        XMVectorGetX(aabb1Min) <= XMVectorGetX(aabb2Max) &&
        XMVectorGetY(aabb1Min) <= XMVectorGetY(aabb2Max) &&
        XMVectorGetZ(aabb1Min) <= XMVectorGetZ(aabb2Max)
    );
}

WBP_PRIMITIVE_API XMFLOAT3 wbp_primitive::GetCollidedFaceNormal
(
    const wbp_primitive::PrimitiveAABB &aabb1, const DirectX::XMMATRIX &aabb1ConvertMat, 
    const wbp_primitive::PrimitiveAABB &aabb2, const DirectX::XMMATRIX &aabb2ConvertMat, 
    const XMFLOAT3 &movement
){
    XMVECTOR aabb1Min = XMVector3TransformCoord(aabb1.GetMinVec(), aabb1ConvertMat);
    XMVECTOR aabb1Max = XMVector3TransformCoord(aabb1.GetMaxVec(), aabb1ConvertMat);
    wbp_primitive::PrimitiveAABB transformedAABB1(aabb1Min, aabb1Max);

    XMVECTOR aabb2Min = XMVector3TransformCoord(aabb2.GetMinVec(), aabb2ConvertMat);
    XMVECTOR aabb2Max = XMVector3TransformCoord(aabb2.GetMaxVec(), aabb2ConvertMat);
    wbp_primitive::PrimitiveAABB transformedAABB2(aabb2Min, aabb2Max);

    // Get relative position of AABBs
    XMVECTOR relativePos = XMVectorSubtract(transformedAABB2.GetCenterVec(), transformedAABB1.GetCenterVec());

    // Calculate the amount of overlap for each axis
    XMVECTOR overlap = transformedAABB2.GetExtentsVec() + transformedAABB1.GetExtentsVec() - XMVectorAbs(relativePos);

    XMFLOAT3 normal = { 0.0f, 0.0f, 0.0f };

    XMFLOAT3 overlapFloat3;
    XMStoreFloat3(&overlapFloat3, overlap);

    XMFLOAT3 relativeFloat3;
    XMStoreFloat3(&relativeFloat3, relativePos);

    // Detects the smallest overlapping axis
    if (overlapFloat3.x < overlapFloat3.y && overlapFloat3.x < overlapFloat3.z)
    {
        // Collision in X-axis direction
        normal.x = (relativeFloat3.x > 0) ? 1.0f : -1.0f;
        normal.y = 0.0f;
        normal.z = 0.0f;

        // Correction from X component of move vector
        if ((movement.x >= 0.0f && normal.x == 1.0f) || (movement.x <= 0.0f && normal.x == -1.0f))
        {
            // If the movement vector and the normal of the collision surface are in the same direction, 
            // the X component is set to 0.
            normal.x = 0.0f;
        }
    } 
    else if (overlapFloat3.y < overlapFloat3.z)
    {
        // Collision in Y-axis direction
        normal.x = 0.0f;
        normal.y = (relativeFloat3.y > 0) ? 1.0f : -1.0f;
        normal.z = 0.0f;

        // Correction from Y component of move vector
        if ((movement.y >= 0.0f && normal.y == 1.0f) || (movement.y <= 0.0f && normal.y == -1.0f))
        {
            // If the movement vector and the normal of the collision surface are in the same direction,
            // the Y component is set to 0.
            normal.y = 0.0f;
        }
    } 
    else
    {
        // Collision in Z-axis direction
        normal.x = 0.0f;
        normal.y = 0.0f;
        normal.z = (relativeFloat3.z > 0) ? 1.0f : -1.0f;

        // Correction from Z component of move vector
        if ((movement.z >= 0.0f && normal.z == 1.0f) || (movement.z <= 0.0f && normal.z == -1.0f))
        {
            // If the movement vector and the normal of the collision surface are in the same direction,
            // the Z component is set to 0.
            normal.z = 0.0f;
        }
    }

    return normal;
}

WBP_PRIMITIVE_API bool wbp_primitive::IntersectRayAABB
(
    const PrimitiveRay &ray, const PrimitiveAABB &aabb, const DirectX::XMMATRIX &aabbConvertMat, 
    float *tmin, float *tmax
){
    XMVECTOR rayOrigin = ray.GetOriginVec();
    XMVECTOR rayDir = XMVector3Normalize(ray.GetDirectionVec());
    float rayLength = ray.GetLength();

    XMVECTOR boxMin = XMVector3TransformCoord(aabb.GetMinVec(), aabbConvertMat);
    XMVECTOR boxMax = XMVector3TransformCoord(aabb.GetMaxVec(), aabbConvertMat);

    float t0 = 0.0f;
    float t1 = rayLength;

    // Judgment by slab method in each axis
    for (int i = 0; i < 3; ++i) 
    {
        float o = XMVectorGetByIndex(rayOrigin, i);
        float d = XMVectorGetByIndex(rayDir, i);
        float minB = XMVectorGetByIndex(boxMin, i);
        float maxB = XMVectorGetByIndex(boxMax, i);

        if (fabsf(d) < 1e-8f) 
        {
            // If the ray is parallel to the axial direction, it will not intersect if outside the range of AABB
            if (o < minB || o > maxB)
            {
                // No intersection
                return false;
            }
        } 
        else 
        {
            float invD = 1.0f / d;
            float tNear = (minB - o) * invD;
            float tFar = (maxB - o) * invD;
            if (tNear > tFar) std::swap(tNear, tFar);
            t0 = tNear > t0 ? tNear : t0;
            t1 = tFar < t1 ? tFar : t1;
            if (t0 > t1)
            {
                // No intersection
                return false;
            }
        }
    }

    if (t1 < 0.0f)
    {
        // Crossing only behind the ray
        return false;
    }

    if (t0 > rayLength)
    {
        // The intersection point is beyond the ray length
        return false;
    }

    if (tmin) *tmin = t0;
    if (tmax) *tmax = t1;

    return true;
}

WBP_PRIMITIVE_API wbp_primitive::PrimitiveAABB wbp_primitive::CreateAABBFromRay
(
    const PrimitiveRay &ray, const DirectX::XMMATRIX &convertMat
){
    XMVECTOR rayOrigin = ray.GetOriginVec();
    XMVECTOR rayDir = ray.GetDirectionVec();
    float rayLength = ray.GetLength();

    // Calculate the end point of the ray
    XMVECTOR endPoint = XMVectorAdd(rayOrigin, XMVectorScale(rayDir, rayLength));

    // Create AABB from the start and end points
    XMVECTOR aabbMin = XMVector3TransformCoord(XMVectorMin(rayOrigin, endPoint), convertMat);
    XMVECTOR aabbMax = XMVector3TransformCoord(XMVectorMax(rayOrigin, endPoint), convertMat);
    wbp_primitive::PrimitiveAABB transformedAABB(aabbMin, aabbMax);

    return transformedAABB;
}