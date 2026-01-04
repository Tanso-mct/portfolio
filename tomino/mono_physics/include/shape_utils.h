#pragma once
#include "mono_physics/include/dll_config.h"

#include "mono_physics/include/shape.h"

#include <vector>
#include <DirectXMath.h>

namespace mono_physics
{
    /*******************************************************************************************************************
     * Box Utility
    /******************************************************************************************************************/

    MONO_PHYSICS_API ShapeBox CreateBoxFromBoxes(
        const std::vector<ShapeBox> &boxes, const std::vector<DirectX::XMMATRIX> &transforms);

    MONO_PHYSICS_API ShapeBox CreateBoxFromVector(
        const DirectX::XMFLOAT3 &vec, const DirectX::XMFLOAT3 &origin = {0.0f, 0.0f, 0.0f});

    /*******************************************************************************************************************
     * Box x Box Utility
    /******************************************************************************************************************/

    MONO_PHYSICS_API bool IsBoxIntersectBox(
        const ShapeBox &box1, const DirectX::XMMATRIX &box1Transform,
        const ShapeBox &box2, const DirectX::XMMATRIX &box2Transform);

    MONO_PHYSICS_API DirectX::XMFLOAT3 GetCollisionNormalFromCollidedBoxes
    (
        const ShapeBox &runnerBox, const DirectX::XMMATRIX &runnerBoxTransform,
        const ShapeBox &receiverBox, const DirectX::XMMATRIX &receiverBoxTransform,
        const DirectX::XMFLOAT3 runnerVelocity
    );

    /*******************************************************************************************************************
     * Sphere Utility
    /******************************************************************************************************************/

    MONO_PHYSICS_API ShapeSphere CreateSphereFromSpheres(
        const std::vector<ShapeSphere> &spheres, const std::vector<DirectX::XMMATRIX> &transforms);

    MONO_PHYSICS_API ShapeSphere CreateSphereFromVector(
        const DirectX::XMFLOAT3 &vec, const DirectX::XMFLOAT3 &origin = {0.0f, 0.0f, 0.0f});

    /*******************************************************************************************************************
     * Sphere x Sphere Utility
    /******************************************************************************************************************/

    MONO_PHYSICS_API bool IsSphereIntersectSphere(const ShapeSphere &sphere1, const ShapeSphere &sphere2);

    MONO_PHYSICS_API DirectX::XMFLOAT3 GetCollisionNormalFromCollidedSpheres
    (
        const ShapeSphere &runnerSphere, const DirectX::XMMATRIX &runnerSphereTransform,
        const ShapeSphere &receiverSphere, const DirectX::XMMATRIX &receiverSphereTransform,
        const DirectX::XMFLOAT3 runnerVelocity
    );

    /*******************************************************************************************************************
     * Ray Utility
    /******************************************************************************************************************/

    MONO_PHYSICS_API ShapeRay CreateRayFromPoints(const DirectX::XMFLOAT3 &start, const DirectX::XMFLOAT3 &end);

    MONO_PHYSICS_API ShapeRay CreateRayFromVector(
        const DirectX::XMFLOAT3 &vec, const DirectX::XMFLOAT3 &origin = {0.0f, 0.0f, 0.0f});

    /*******************************************************************************************************************
     * Ray x Box Utility
    /******************************************************************************************************************/

    MONO_PHYSICS_API bool IsRayIntersectBox(
        const ShapeRay &ray, const ShapeBox &box, DirectX::XMFLOAT3 *outHitPoint = nullptr);
        
    MONO_PHYSICS_API DirectX::XMFLOAT3 GetHitPointFromRayAndBox(const ShapeRay &ray, const ShapeBox &box);


} // namespace mono_physics