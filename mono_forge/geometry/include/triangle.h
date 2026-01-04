#pragma once

#include "geometry/include/dll_config.h"
#include "geometry/include/geometry.h"

namespace geometry
{

class GEOMETRY_DLL Triangle :
    public Geometry
{
public:
    // Construct a triangle with given vertices, normals, and UVs
    Triangle(
        const DirectX::XMFLOAT3& v0, const DirectX::XMFLOAT2& uv0, const DirectX::XMFLOAT3& n0, const DirectX::XMFLOAT3& t0,
        const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT2& uv1, const DirectX::XMFLOAT3& n1, const DirectX::XMFLOAT3& t1,
        const DirectX::XMFLOAT3& v2, const DirectX::XMFLOAT2& uv2, const DirectX::XMFLOAT3& n2, const DirectX::XMFLOAT3& t2);

    ~Triangle() override = default;
};

} // namespace geometry