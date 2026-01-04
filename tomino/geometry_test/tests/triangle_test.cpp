#include "geometry_test/pch.h"

#include "geometry/include/triangle.h"
using namespace DirectX;

TEST(Triangle, Create)
{
    // Create a triangle with specified vertices, normals, and UVs
    geometry::Triangle triangle(
        XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
        XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
        XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));

    // Verify vertices
    const std::vector<geometry::Geometry::Vertex>& vertices = triangle.GetVertices();
    EXPECT_EQ(vertices.size(), 3);

    // Verify indices
    const std::vector<uint32_t>& indices = triangle.GetIndices();
    EXPECT_EQ(indices.size(), 3);

    // Get vertex data pointers
    const geometry::Geometry::Vertex* vertex_data = triangle.GetVertexData();
    EXPECT_NE(vertex_data, nullptr);

    // Get index data pointers
    const uint32_t* index_data = triangle.GetIndexData();
    EXPECT_NE(index_data, nullptr);

    // Clean up
    triangle.ClearVertices();
    triangle.ClearIndices();
    EXPECT_EQ(triangle.GetVertices().size(), 0);
    EXPECT_EQ(triangle.GetIndices().size(), 0);
}