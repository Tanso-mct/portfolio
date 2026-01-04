#include "mono_physics/src/pch.h"
#include "mono_physics/include/shape.h"

using namespace DirectX;

namespace shape_util
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

} // namespace shape_util

MONO_PHYSICS_API size_t mono_physics::CreateShapeTypeID()
{
    static size_t typeIDCounter = 0;
    return typeIDCounter++;
}

mono_physics::ShapeBox::ShapeBox()
{
}

mono_physics::ShapeBox::ShapeBox(const DirectX::XMFLOAT3 &center, const DirectX::XMFLOAT3 &extents)
{
    center_ = center;
    extents_ = extents;
    shape_util::UpdateMin(min_, center_, extents_);
    shape_util::UpdateMax(max_, center_, extents_);
}

mono_physics::ShapeBox::~ShapeBox()
{
}

size_t mono_physics::ShapeBox::GetTypeID() const
{
    static size_t typeID = CreateShapeTypeID();
    return typeID;
}

void mono_physics::ShapeBox::SetMin(const XMFLOAT3 &min)
{
    min_ = min;
    shape_util::UpdateCenter(center_, min_, max_);
    shape_util::UpdateExtents(extents_, min_, max_);
}

void mono_physics::ShapeBox::SetMax(const XMFLOAT3 &max)
{
    max_ = max;
    shape_util::UpdateCenter(center_, min_, max_);
    shape_util::UpdateExtents(extents_, min_, max_);
}

void mono_physics::ShapeBox::SetCenter(const XMFLOAT3 &center)
{
    center_ = center;
    shape_util::UpdateMin(min_, center_, extents_);
    shape_util::UpdateMax(max_, center_, extents_);
}

void mono_physics::ShapeBox::SetExtents(const XMFLOAT3& extents)
{
    extents_ = extents;
    shape_util::UpdateMin(min_, center_, extents_);
    shape_util::UpdateMax(max_, center_, extents_);
}

mono_physics::ShapeSphere::ShapeSphere()
{
}

mono_physics::ShapeSphere::ShapeSphere(const DirectX::XMFLOAT3 &center, float radius)
{
}

mono_physics::ShapeSphere::~ShapeSphere()
{
}

size_t mono_physics::ShapeSphere::GetTypeID() const
{
    return size_t();
}

void mono_physics::ShapeSphere::SetCenter(const DirectX::XMFLOAT3 &center)
{
}

void mono_physics::ShapeSphere::SetRadius(float radius)
{
}

mono_physics::ShapeRay::ShapeRay()
{
}

mono_physics::ShapeRay::ShapeRay(const DirectX::XMFLOAT3 &origin, const DirectX::XMFLOAT3 &direction, float length)
{
}

mono_physics::ShapeRay::~ShapeRay()
{
}

size_t mono_physics::ShapeRay::GetTypeID() const
{
    return size_t();
}

void mono_physics::ShapeRay::SetOrigin(const DirectX::XMFLOAT3 &origin)
{
}

void mono_physics::ShapeRay::SetDirection(const DirectX::XMFLOAT3 &direction)
{
}

void mono_physics::ShapeRay::SetLength(float length)
{
}