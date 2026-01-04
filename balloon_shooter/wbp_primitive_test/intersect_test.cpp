#include "pch.h"

#include "wbp_primitive/include/primitive_helpers.h"
#pragma comment(lib, "wbp_primitive.lib")

using namespace DirectX;

TEST(Primitive, IntersectRayAABB)
{
    XMFLOAT3 origin(0.0f, 0.0f, -2.0f);
    XMFLOAT3 direction(0.0f, 0.0f, 10.0f);
    wbp_primitive::PrimitiveRay ray(origin, direction);
    ray.SetLength(10.0f);

    XMFLOAT3 aabbMin(-0.5f, -0.5f, -0.5f);
    XMFLOAT3 aabbMax(0.5f, 0.5f, 0.5f);
    wbp_primitive::PrimitiveAABB box(aabbMin, aabbMax);

    float tmin, tmax;
    if (IntersectRayAABB(ray, box, XMMatrixIdentity(), &tmin, &tmax)) 
    {
        std::cout << "Ray intersects AABB at tmin: " << tmin << ", tmax: " << tmax << std::endl;
    } 
    else 
    {
        FAIL() << "Ray did not intersect AABB as expected.";
    }
}