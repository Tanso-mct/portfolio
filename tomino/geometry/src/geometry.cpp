#include "geometry/src/pch.h"
#include "geometry/include/geometry.h"

namespace geometry
{

void Geometry::ClearVertices()
{
    vertices_.clear();
}

void Geometry::ClearIndices()
{
    indices_.clear();
}

void Geometry::AddVertex(Vertex&& vertex)
{
    vertices_.emplace_back(std::move(vertex));
}

void Geometry::AddIndex(Index index)
{
    indices_.emplace_back(index);
}

} // namespace geometry