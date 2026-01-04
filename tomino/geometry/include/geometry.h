#pragma once

#include <stdint.h>
#include <DirectXMath.h>
#include <vector>

#include "geometry/include/dll_config.h"

namespace geometry
{

class GEOMETRY_DLL Geometry
{
public:
    Geometry() = default;
    virtual ~Geometry() = default;

    // Vertex structure
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 texcoord;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT3 tangent;
    };

    // Index type
    using Index = uint32_t;

    const std::vector<Vertex>& GetVertices() const { return vertices_; }
    const Vertex* GetVertexData() const { return vertices_.data(); }
    size_t GetVertexCount() const { return vertices_.size(); }
    size_t GetVertexSize() const { return sizeof(Vertex); }
    void ClearVertices();

    const std::vector<Index>& GetIndices() const { return indices_; }
    const Index* GetIndexData() const { return indices_.data(); }
    size_t GetIndexCount() const { return indices_.size(); }
    size_t GetIndexSize() const { return sizeof(Index); }
    void ClearIndices();

protected:
    void AddVertex(Vertex&& vertex);
    void AddIndex(Index index);

private:
    std::vector<Vertex> vertices_;
    std::vector<Index> indices_;
};

} // namespace geometry