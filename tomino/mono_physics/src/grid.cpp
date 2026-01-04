#include "mono_physics/src/pch.h"
#include "mono_physics/include/grid.h"

mono_physics::GridIndex mono_physics::SpatialGrid::CalcGridIndex(const DirectX::XMFLOAT3 &pos) const
{
    return 
    {
        static_cast<int>(pos.x / gridSize),
        static_cast<int>(pos.y / gridSize),
        static_cast<int>(pos.z / gridSize)
    };
}

std::vector<mono_physics::GridIndex> mono_physics::SpatialGrid::CalcCoveredCells(
    const DirectX::XMFLOAT3& min, const DirectX::XMFLOAT3& max) const
{
    std::vector<GridIndex> cells;

    int minX = static_cast<int>(min.x / gridSize);
    int minY = static_cast<int>(min.y / gridSize);
    int minZ = static_cast<int>(min.z / gridSize);
    int maxX = static_cast<int>(max.x / gridSize);
    int maxY = static_cast<int>(max.y / gridSize);
    int maxZ = static_cast<int>(max.z / gridSize);

    for (int x = minX; x <= maxX; ++x)
        for (int y = minY; y <= maxY; ++y)
            for (int z = minZ; z <= maxZ; ++z)
                cells.push_back({x, y, z});

    return cells;
}

void mono_physics::SpatialGrid::RegisterAABB(
    const riaecs::Entity &entity, const mono_physics::ShapeBox &aabb, const DirectX::XMMATRIX& worldMatrixNoRot)
{
    // Transform AABB min by world matrix
    DirectX::XMFLOAT3 transformedMin;
    DirectX::XMStoreFloat3(&transformedMin, DirectX::XMVector3Transform(
        DirectX::XMLoadFloat3(&aabb.GetMin()), worldMatrixNoRot));

    // Transform AABB max by world matrix
    DirectX::XMFLOAT3 transformedMax;
    DirectX::XMStoreFloat3(&transformedMax, DirectX::XMVector3Transform(
        DirectX::XMLoadFloat3(&aabb.GetMax()), worldMatrixNoRot));

    std::vector<GridIndex> cells = CalcCoveredCells(transformedMin, transformedMax);
    for (const GridIndex& cell : cells) 
        gridMap[cell].push_back(entity);
}

std::vector<mono_physics::GridIndex> mono_physics::SpatialGrid::GetNeighborCells(
    const mono_physics::ShapeBox &aabb, const DirectX::XMMATRIX& worldMatrixNoRot) const
{
    // Transform AABB min by world matrix
    DirectX::XMFLOAT3 transformedMin;
    DirectX::XMStoreFloat3(&transformedMin, DirectX::XMVector3Transform(
        DirectX::XMLoadFloat3(&aabb.GetMin()), worldMatrixNoRot));

    // Transform AABB max by world matrix
    DirectX::XMFLOAT3 transformedMax;
    DirectX::XMStoreFloat3(&transformedMax, DirectX::XMVector3Transform(
        DirectX::XMLoadFloat3(&aabb.GetMax()), worldMatrixNoRot));

    std::vector<GridIndex> cells = CalcCoveredCells(transformedMin, transformedMax);
    std::vector<GridIndex> neighbors;
    for (const GridIndex& cell : cells) 
        for (int dx = -1; dx <= 1; ++dx)
            for (int dy = -1; dy <= 1; ++dy)
                for (int dz = -1; dz <= 1; ++dz)
                    neighbors.push_back({cell.x + dx, cell.y + dy, cell.z + dz});

    return neighbors;
}

std::vector<riaecs::Entity> mono_physics::SpatialGrid::QueryNearby(
    const riaecs::Entity &self, const mono_physics::ShapeBox &aabb, const DirectX::XMMATRIX& worldMatrixNoRot) const
{
    // Transform AABB min by world matrix
    DirectX::XMFLOAT3 transformedMin;
    DirectX::XMStoreFloat3(&transformedMin, DirectX::XMVector3Transform(
        DirectX::XMLoadFloat3(&aabb.GetMin()), worldMatrixNoRot));

    // Transform AABB max by world matrix
    DirectX::XMFLOAT3 transformedMax;
    DirectX::XMStoreFloat3(&transformedMax, DirectX::XMVector3Transform(
        DirectX::XMLoadFloat3(&aabb.GetMax()), worldMatrixNoRot));

    std::unordered_set<riaecs::Entity> uniqueIDs;
    std::vector<GridIndex> neighborCells = GetNeighborCells(aabb, worldMatrixNoRot);

    for (const GridIndex& cell : neighborCells)
    {
        auto it = gridMap.find(cell);
        if (it != gridMap.end())
        {
            for (const riaecs::Entity &id : it->second)
            {
                if (id != self)
                    uniqueIDs.insert(id);
            }
        }
    }

    return std::vector<riaecs::Entity>(uniqueIDs.begin(), uniqueIDs.end());
}

void mono_physics::SpatialGrid::Clear()
{
    gridMap.clear();
}