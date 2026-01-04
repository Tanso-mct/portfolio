#include "mono_physics_test/pch.h"

#include "mono_physics/include/grid.h"
#pragma comment(lib, "mono_physics.lib")

using namespace DirectX;

TEST(Grid, Register)
{
    mono_physics::SpatialGrid grid(1.0f);

    DirectX::XMFLOAT3 center = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 extent = { 0.5f, 0.5f, 0.5f };
    mono_physics::ShapeBox aabb(center, extent);

    riaecs::Entity entity1 = riaecs::Entity(0, 0);
    grid.RegisterAABB(entity1, aabb, DirectX::XMMatrixIdentity());

    riaecs::Entity entity2 = riaecs::Entity(1, 0);
    grid.RegisterAABB(entity2, aabb, DirectX::XMMatrixTranslation(0.5f, 0.5f, 0.5f));

    std::vector<mono_physics::GridIndex> neighborCells = grid.GetNeighborCells(aabb, DirectX::XMMatrixIdentity());
    EXPECT_FALSE(neighborCells.empty());

    std::vector<riaecs::Entity> nearbyEntities = grid.QueryNearby(entity1, aabb, DirectX::XMMatrixIdentity());
    EXPECT_EQ(nearbyEntities.size(), 1);
    EXPECT_EQ(nearbyEntities[0], entity2);

    grid.Clear();
}