#include "pch.h"

#include "wbp_primitive/include/grid.h"
#pragma comment(lib, "wbp_primitive.lib")

#include <DirectXMath.h>
using namespace DirectX;

#include <algorithm>

TEST(Grid, RegisterAndQuery)
{
    std::vector<wbp_primitive::PrimitiveAABB> aabbs;
    aabbs.emplace_back(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(5.0f, 5.0f, 5.0f));
    aabbs.emplace_back(XMFLOAT3(3.0f, 3.0f, 3.0f), XMFLOAT3(8.0f, 8.0f, 8.0f));
    aabbs.emplace_back(XMFLOAT3(50.0f, 50.0f, 50.0f), XMFLOAT3(60.0f, 60.0f, 60.0f));

    float gridSize = -FLT_MAX;
    for (const auto& aabb : aabbs)
    {
        float size = std::max({aabb.GetSize().x, aabb.GetSize().y, aabb.GetSize().z});
        gridSize = std::max(gridSize, size);
    }
    
    wbp_primitive::SpatialGrid grid(gridSize);

    // Register AABBs with IDs
    for (size_t i = 0; i < aabbs.size(); ++i)
    {
        grid.RegisterAABB(i, aabbs[i]);
    }

    // Query nearby entities
    std::vector<size_t> nearby = grid.QueryNearby(1, aabbs[0]);
    EXPECT_EQ(nearby.size(), 1);
    EXPECT_EQ(nearby[0], 0);

    // Check neighbor cells
    std::vector<wbp_primitive::GridIndex> neighborCells = grid.GetNeighborCells(aabbs[0]);
    EXPECT_FALSE(neighborCells.empty());
}