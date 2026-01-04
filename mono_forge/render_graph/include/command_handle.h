#pragma once

#include <vector>

#include "utility_header/id.h"

namespace render_graph
{

// The type used to identify command set handles
using CommandSetHandle = utility_header::GenerationID;

// The vector of command set handles
using CommandSetHandles = std::vector<CommandSetHandle>;

} // namespace render_graph