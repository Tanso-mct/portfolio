#include "wbp_primitive/src/pch.h"
#include "wbp_primitive/include/primitive.h"

using namespace DirectX;

namespace
{
    void UpdateCenter
    (
        XMFLOAT3 &center,
        const XMFLOAT3 &min, const XMFLOAT3 &max
    ){
        center.x = min.x + (max.x - min.x) * 0.5f;
        center.y = min.y + (max.y - min.y) * 0.5f;
        center.z = min.z + (max.z - min.z) * 0.5f;
    }

    void UpdateExtents
    (
        XMFLOAT3 &extents,
        const XMFLOAT3 &min, const XMFLOAT3 &max
    ){
        extents.x = (max.x - min.x) * 0.5f;
        extents.y = (max.y - min.y) * 0.5f;
        extents.z = (max.z - min.z) * 0.5f;
    }

    void UpdateMin
    (
        XMFLOAT3 &min,
        const XMFLOAT3 &center, const XMFLOAT3 &extents
    ){
        min.x = center.x - extents.x;
        min.y = center.y - extents.y;
        min.z = center.z - extents.z;
    }

    void UpdateMax
    (
        XMFLOAT3 &max,
        const XMFLOAT3 &center, const XMFLOAT3 &extents
    ){
        max.x = center.x + extents.x;
        max.y = center.y + extents.y;
        max.z = center.z + extents.z;
    }

    void UpdateSize
    (
        XMFLOAT3 &size,
        const XMFLOAT3 &extents
    ){
        size.x = extents.x * 2.0f;
        size.y = extents.y * 2.0f;
        size.z = extents.z * 2.0f;
    }

} // namespace

wbp_primitive::PrimitiveAABB::PrimitiveAABB()
{
    min_ = XMFLOAT3(0.0f, 0.0f, 0.0f);
    max_ = XMFLOAT3(0.0f, 0.0f, 0.0f);
    center_ = XMFLOAT3(0.0f, 0.0f, 0.0f);
    extents_ = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

wbp_primitive::PrimitiveAABB::PrimitiveAABB(const XMFLOAT3 &min, const XMFLOAT3 &max)
{
    min_ = min;
    max_ = max;
    UpdateCenter(center_, min_, max_);
    UpdateExtents(extents_, min_, max_);
    UpdateSize(size_, extents_);
}

wbp_primitive::PrimitiveAABB::PrimitiveAABB(DirectX::XMVECTOR minVec, DirectX::XMVECTOR maxVec)
{
    XMStoreFloat3(&min_, minVec);
    XMStoreFloat3(&max_, maxVec);
    UpdateCenter(center_, min_, max_);
    UpdateExtents(extents_, min_, max_);
    UpdateSize(size_, extents_);
}

void wbp_primitive::PrimitiveAABB::SetMin(const XMFLOAT3 &min)
{
    min_ = min;
    UpdateCenter(center_, min_, max_);
    UpdateExtents(extents_, min_, max_);
    UpdateSize(size_, extents_);
}

void wbp_primitive::PrimitiveAABB::SetMax(const XMFLOAT3 &max)
{
    max_ = max;
    UpdateCenter(center_, min_, max_);
    UpdateExtents(extents_, min_, max_);
    UpdateSize(size_, extents_);
}

void wbp_primitive::PrimitiveAABB::SetCenter(const XMFLOAT3 &center)
{
    center_ = center;
    UpdateMin(min_, center_, extents_);
    UpdateMax(max_, center_, extents_);
}

void wbp_primitive::PrimitiveAABB::SetExtents(const XMFLOAT3 &extents)
{
    extents_ = extents;
    UpdateMin(min_, center_, extents_);
    UpdateMax(max_, center_, extents_);
    UpdateSize(size_, extents_);
}

void wbp_primitive::PrimitiveAABB::SetSize(const DirectX::XMFLOAT3 &size)
{
    if (size.x < 0.0f || size.y < 0.0f || size.z < 0.0f)
    {
        throw std::invalid_argument("Size cannot be negative");
    }

    size_ = size;
    extents_.x = size.x * 0.5f;
    extents_.y = size.y * 0.5f;
    extents_.z = size.z * 0.5f;

    UpdateMin(min_, center_, extents_);
    UpdateMax(max_, center_, extents_);
}

wbp_primitive::PrimitiveRay::PrimitiveRay()
{
    origin_ = XMFLOAT3(0.0f, 0.0f, 0.0f);
    direction_ = DEFAULT_DIR;
}

wbp_primitive::PrimitiveRay::PrimitiveRay(const XMFLOAT3 &origin, const XMFLOAT3 &direction)
{
    origin_ = origin;

    XMVECTOR dirVec = XMVector3Normalize(XMLoadFloat3(&direction));
    XMStoreFloat3(&direction_, dirVec);
}

void wbp_primitive::PrimitiveRay::SetDirection(const DirectX::XMFLOAT3 &direction)
{
    XMVECTOR dirVec = XMVector3Normalize(XMLoadFloat3(&direction));
    XMStoreFloat3(&direction_, dirVec);
}