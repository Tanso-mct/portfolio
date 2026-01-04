#include "mono_physics/src/pch.h"
#include "mono_physics/include/shape_utils.h"

using namespace DirectX;

MONO_PHYSICS_API mono_physics::ShapeBox mono_physics::CreateBoxFromBoxes(
    const std::vector<mono_physics::ShapeBox> &boxes, const std::vector<XMMATRIX> &transforms)
{
    return mono_physics::ShapeBox();
}

MONO_PHYSICS_API mono_physics::ShapeBox mono_physics::CreateBoxFromVector(
    const XMFLOAT3 &vec, const XMFLOAT3 &origin)
{
    return mono_physics::ShapeBox();
}

MONO_PHYSICS_API bool mono_physics::IsBoxIntersectBox(
    const ShapeBox &box1, const XMMATRIX &box1Transform,
    const ShapeBox &box2, const XMMATRIX &box2Transform)
{
    // Get transformed min and max points of box1
    XMFLOAT3 box1MinTransformed, box1MaxTransformed;
    XMStoreFloat3(&box1MinTransformed, XMVector3Transform(XMLoadFloat3(&box1.GetMin()), box1Transform));
    XMStoreFloat3(&box1MaxTransformed, XMVector3Transform(XMLoadFloat3(&box1.GetMax()), box1Transform));

    // Get transformed min and max points of box2
    XMFLOAT3 box2MinTransformed, box2MaxTransformed;
    XMStoreFloat3(&box2MinTransformed, XMVector3Transform(XMLoadFloat3(&box2.GetMin()), box2Transform));
    XMStoreFloat3(&box2MaxTransformed, XMVector3Transform(XMLoadFloat3(&box2.GetMax()), box2Transform));

    // Check for overlap on all axes
    bool overlapX = (box1MinTransformed.x <= box2MaxTransformed.x) && (box1MaxTransformed.x >= box2MinTransformed.x);
    bool overlapY = (box1MinTransformed.y <= box2MaxTransformed.y) && (box1MaxTransformed.y >= box2MinTransformed.y);
    bool overlapZ = (box1MinTransformed.z <= box2MaxTransformed.z) && (box1MaxTransformed.z >= box2MinTransformed.z);

    return overlapX && overlapY && overlapZ; // Boxes intersect if they overlap on all axes
}

MONO_PHYSICS_API XMFLOAT3 mono_physics::GetCollisionNormalFromCollidedBoxes
(
    const mono_physics::ShapeBox &runnerBox, const XMMATRIX &runnerBoxTransform, 
    const mono_physics::ShapeBox &receiverBox, const XMMATRIX &receiverBoxTransform, 
    const XMFLOAT3 runnerVelocity
){
    // Get transformed min and max points of runnerBox
    XMFLOAT3 runnerMinTransformed, runnerMaxTransformed;
    XMStoreFloat3(&runnerMinTransformed, XMVector3Transform(XMLoadFloat3(&runnerBox.GetMin()), runnerBoxTransform));
    XMStoreFloat3(&runnerMaxTransformed, XMVector3Transform(XMLoadFloat3(&runnerBox.GetMax()), runnerBoxTransform));

    // Create transformed AABBs
    ShapeBox runnerTransformedBox = ShapeBox();
    runnerTransformedBox.SetMin(runnerMinTransformed);
    runnerTransformedBox.SetMax(runnerMaxTransformed);

    // Get transformed min and max points of receiverBox
    XMFLOAT3 receiverMinTransformed, receiverMaxTransformed;
    XMStoreFloat3(&receiverMinTransformed, XMVector3Transform(XMLoadFloat3(&receiverBox.GetMin()), receiverBoxTransform));
    XMStoreFloat3(&receiverMaxTransformed, XMVector3Transform(XMLoadFloat3(&receiverBox.GetMax()), receiverBoxTransform));

    ShapeBox receiverTransformedBox = ShapeBox();
    receiverTransformedBox.SetMin(receiverMinTransformed);
    receiverTransformedBox.SetMax(receiverMaxTransformed);

    // Get relative position of AABBs
    XMVECTOR runnerTransformedCenterVec = XMLoadFloat3(&runnerTransformedBox.GetCenter());
    XMVECTOR receiverTransformedCenterVec = XMLoadFloat3(&receiverTransformedBox.GetCenter());
    XMVECTOR relativePos = XMVectorSubtract(runnerTransformedCenterVec, receiverTransformedCenterVec);

    // Calculate the amount of overlap for each axis
    XMVECTOR runnerTransformedExtentsVec = XMLoadFloat3(&runnerTransformedBox.GetExtents());
    XMVECTOR receiverTransformedExtentsVec = XMLoadFloat3(&receiverTransformedBox.GetExtents());
    XMVECTOR overlap = receiverTransformedExtentsVec + runnerTransformedExtentsVec - XMVectorAbs(relativePos);

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
        if ((runnerVelocity.x >= 0.0f && normal.x == 1.0f) || (runnerVelocity.x <= 0.0f && normal.x == -1.0f))
        {
            // If the runnerVelocity vector and the normal of the collision surface are in the same direction, 
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
        if ((runnerVelocity.y >= 0.0f && normal.y == 1.0f) || (runnerVelocity.y <= 0.0f && normal.y == -1.0f))
        {
            // If the runnerVelocity vector and the normal of the collision surface are in the same direction,
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
        if ((runnerVelocity.z >= 0.0f && normal.z == 1.0f) || (runnerVelocity.z <= 0.0f && normal.z == -1.0f))
        {
            // If the runnerVelocity vector and the normal of the collision surface are in the same direction,
            // the Z component is set to 0.
            normal.z = 0.0f;
        }
    }

    return normal;
}

MONO_PHYSICS_API mono_physics::ShapeSphere mono_physics::CreateSphereFromSpheres(
    const std::vector<mono_physics::ShapeSphere> &spheres, const std::vector<XMMATRIX> &transforms)
{
    return mono_physics::ShapeSphere();
}

MONO_PHYSICS_API mono_physics::ShapeSphere mono_physics::CreateSphereFromVector(
    const XMFLOAT3 &vec, const XMFLOAT3 &origin)
{
    return mono_physics::ShapeSphere();
}

MONO_PHYSICS_API bool mono_physics::IsSphereIntersectSphere(
    const mono_physics::ShapeSphere &sphere1, const mono_physics::ShapeSphere &sphere2)
{
    return false;
}

MONO_PHYSICS_API XMFLOAT3 mono_physics::GetCollisionNormalFromCollidedSpheres
(
    const mono_physics::ShapeSphere &runnerSphere, const XMMATRIX &runnerSphereTransform, 
    const mono_physics::ShapeSphere &receiverSphere, const XMMATRIX &receiverSphereTransform, 
    const XMFLOAT3 runnerVelocity
){
    return XMFLOAT3();
}

MONO_PHYSICS_API mono_physics::ShapeRay mono_physics::CreateRayFromPoints(
    const XMFLOAT3 &start, const XMFLOAT3 &end)
{
    return mono_physics::ShapeRay();
}

MONO_PHYSICS_API mono_physics::ShapeRay mono_physics::CreateRayFromVector(
    const XMFLOAT3 &vec, const XMFLOAT3 &origin)
{
    return mono_physics::ShapeRay();
}

MONO_PHYSICS_API bool mono_physics::IsRayIntersectBox(
    const mono_physics::ShapeRay &ray, const mono_physics::ShapeBox &box, XMFLOAT3 *outHitPoint)
{
    return false;
}

MONO_PHYSICS_API XMFLOAT3 mono_physics::GetHitPointFromRayAndBox(
    const mono_physics::ShapeRay &ray, const mono_physics::ShapeBox &box)
{
    return XMFLOAT3();
}