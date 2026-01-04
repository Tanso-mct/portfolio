#pragma once
#include "mono_physics/include/dll_config.h"
#include "riaecs/riaecs.h"

#include "mono_physics/include/shape.h"

#include <DirectXMath.h>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace mono_physics
{
    struct GridIndex 
    {
        int x, y, z;

        bool operator==(const GridIndex& other) const 
        {
            return x == other.x && y == other.y && z == other.z;
        }
    };

} // namespace mono_physics

namespace std 
{
    template <>
    struct hash<mono_physics::GridIndex> 
    {
        inline std::size_t operator()(const mono_physics::GridIndex& idx) const 
        {
            std::size_t h1 = std::hash<int>()(idx.x);
            std::size_t h2 = std::hash<int>()(idx.y);
            std::size_t h3 = std::hash<int>()(idx.z);

            std::size_t seed = h1;
            seed ^= h2 + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
            seed ^= h3 + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
            return seed;
        }
    };
}

namespace mono_physics
{

    class MONO_PHYSICS_API SpatialGrid 
    {
    private:
        float gridSize;

        // Stores object ID for each cell
        std::unordered_map<GridIndex, std::vector<riaecs::Entity>> gridMap;

    public:
        SpatialGrid(float gridSize_) : gridSize(gridSize_) {}
        ~SpatialGrid() = default;

        GridIndex CalcGridIndex(const DirectX::XMFLOAT3& pos) const;
        std::vector<GridIndex> CalcCoveredCells(const DirectX::XMFLOAT3& min, const DirectX::XMFLOAT3& max) const;

        void RegisterAABB(
            const riaecs::Entity &entity, const ShapeBox& aabb, const DirectX::XMMATRIX& worldMatrixNoRot);

        std::vector<GridIndex> GetNeighborCells(
            const ShapeBox& aabb, const DirectX::XMMATRIX& worldMatrixNoRot) const;

        std::vector<riaecs::Entity> QueryNearby(
            const riaecs::Entity &self, const ShapeBox& aabb, const DirectX::XMMATRIX& worldMatrixNoRot) const;

        void Clear();
    };

} // namespace mono_physics