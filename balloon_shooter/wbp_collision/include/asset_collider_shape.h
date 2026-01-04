#pragma once
#include "wbp_collision/include/dll_config.h"
#include "windows_base/windows_base.h"

#include "wbp_collision/include/interfaces/asset_collider_shape.h"

namespace wbp_collision
{
    class ColliderShapeAsset : public IColliderShapeAsset
    {
    private:
        std::vector<DirectX::XMFLOAT3> vertices_;
        std::vector<wbp_primitive::PrimitiveAABB> aabbs_;
        wbp_primitive::PrimitiveAABB totalAABB_;

    public:
        ColliderShapeAsset() = default;
        ~ColliderShapeAsset() override = default;

        std::vector<DirectX::XMFLOAT3> &GetVertices() override { return vertices_; }

        std::vector<wbp_primitive::PrimitiveAABB> &GetAABBs() override { return aabbs_; }
        wbp_primitive::PrimitiveAABB &GetTotalAABB() override { return totalAABB_; }
    };

} // namespace wbp_collision