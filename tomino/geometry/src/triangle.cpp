#include "geometry/src/pch.h"
#include "geometry/include/triangle.h"

using namespace DirectX;

namespace geometry
{

geometry::Triangle::Triangle(
    const XMFLOAT3 &v0, const XMFLOAT2 &uv0, const XMFLOAT3 &n0, const XMFLOAT3 &t0,
    const XMFLOAT3 &v1, const XMFLOAT2 &uv1, const XMFLOAT3 &n1, const XMFLOAT3 &t1,
    const XMFLOAT3 &v2, const XMFLOAT2 &uv2, const XMFLOAT3 &n2, const XMFLOAT3 &t2)
{
    // Add vertices
    AddVertex({ v0, uv0, n0, t0 });
    AddVertex({ v1, uv1, n1, t1 });
    AddVertex({ v2, uv2, n2, t2 });

    // Add indices
    AddIndex(0);
    AddIndex(1);
    AddIndex(2);
}

} // namespace geometry