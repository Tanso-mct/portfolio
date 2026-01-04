#pragma once
#include "wbp_primitive/include/dll_config.h"

#include "wbp_primitive/include/primitive.h"

#include <DirectXMath.h>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace wbp_primitive
{
    struct GridIndex 
    {
        int x, y, z;

        bool operator==(const GridIndex& other) const 
        {
            return x == other.x && y == other.y && z == other.z;
        }
    };

} // namespace wbp_primitive

namespace std 
{
    template <>
    struct hash<wbp_primitive::GridIndex> 
    {
        std::size_t operator()(const wbp_primitive::GridIndex& idx) const 
        {
            return ((std::hash<int>()(idx.x) ^ (std::hash<int>()(idx.y) << 1)) >> 1) ^ (std::hash<int>()(idx.z) << 1);
        }
    };
}

namespace wbp_primitive
{

    class WBP_PRIMITIVE_API SpatialGrid 
    {
    private:
        float gridSize;

        // Stores object ID for each cell
        std::unordered_map<GridIndex, std::vector<size_t>> gridMap;

    public:
        SpatialGrid(float gridSize_) : gridSize(gridSize_) {}
        ~SpatialGrid() = default;

        GridIndex CalcGridIndex(const DirectX::XMFLOAT3& pos) const;
        std::vector<GridIndex> CalcCoveredCells(const PrimitiveAABB& aabb) const;

        void RegisterAABB(size_t entityID, const PrimitiveAABB& aabb);

        std::vector<GridIndex> GetNeighborCells(const PrimitiveAABB& aabb) const;
        std::vector<size_t> QueryNearby(size_t selfID, const PrimitiveAABB& aabb) const;

        void Clear();
    };

} // namespace wbp_primitive