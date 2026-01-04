#pragma once

#include <vector>

#include "utility_header/id.h"

namespace render_graph
{

// The type used to identify resource handles
using ResourceHandle = utility_header::GenerationID;
using ResourceHandles = std::vector<ResourceHandle>;

} // namespace render_graph