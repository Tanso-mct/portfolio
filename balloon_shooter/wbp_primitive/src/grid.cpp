#include "wbp_primitive/src/pch.h"
#include "wbp_primitive/include/grid.h"

wbp_primitive::GridIndex wbp_primitive::SpatialGrid::CalcGridIndex(const DirectX::XMFLOAT3 &pos) const
{
    return 
    {
        static_cast<int>(pos.x / gridSize),
        static_cast<int>(pos.y / gridSize),
        static_cast<int>(pos.z / gridSize)
    };
}

std::vector<wbp_primitive::GridIndex> wbp_primitive::SpatialGrid::CalcCoveredCells(const PrimitiveAABB &aabb) const
{
    std::vector<GridIndex> cells;
    const DirectX::XMFLOAT3 &min = aabb.GetMin();
    const DirectX::XMFLOAT3 &max = aabb.GetMax();

    int minX = static_cast<int>(min.x / gridSize);
    int minY = static_cast<int>(min.y / gridSize);
    int minZ = static_cast<int>(min.z / gridSize);
    int maxX = static_cast<int>(max.x / gridSize);
    int maxY = static_cast<int>(max.y / gridSize);
    int maxZ = static_cast<int>(max.z / gridSize);

    for (int x = minX; x <= maxX; ++x)
    {
        for (int y = minY; y <= maxY; ++y)
        {
            for (int z = minZ; z <= maxZ; ++z)
            {
                cells.push_back({x, y, z});
            }
        }
    }

    return cells;
}

void wbp_primitive::SpatialGrid::RegisterAABB(size_t entityID, const PrimitiveAABB &aabb)
{
    std::vector<GridIndex> cells = CalcCoveredCells(aabb);
    for (const GridIndex& cell : cells) 
    {
        gridMap[cell].push_back(entityID);
    }
}

std::vector<wbp_primitive::GridIndex> wbp_primitive::SpatialGrid::GetNeighborCells(const PrimitiveAABB &aabb) const
{
    std::vector<GridIndex> cells = CalcCoveredCells(aabb);
    std::vector<GridIndex> neighbors;
    for (const GridIndex& cell : cells) 
    {
        for (int dx = -1; dx <= 1; ++dx)
        {
            for (int dy = -1; dy <= 1; ++dy)
            {
                for (int dz = -1; dz <= 1; ++dz)
                {
                    neighbors.push_back({cell.x + dx, cell.y + dy, cell.z + dz});
                }
            }
        }
    }

    return neighbors;
}

std::vector<size_t> wbp_primitive::SpatialGrid::QueryNearby(size_t selfID, const PrimitiveAABB &aabb) const
{
    std::unordered_set<size_t> uniqueIDs;
    std::vector<GridIndex> neighborCells = GetNeighborCells(aabb);

    for (const GridIndex& cell : neighborCells)
    {
        auto it = gridMap.find(cell);
        if (it != gridMap.end())
        {
            for (size_t id : it->second)
            {
                if (id != selfID)
                    uniqueIDs.insert(id);
            }
        }
    }

    return std::vector<size_t>(uniqueIDs.begin(), uniqueIDs.end());
}

void wbp_primitive::SpatialGrid::Clear()
{
    gridMap.clear();
}