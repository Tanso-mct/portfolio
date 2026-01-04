#pragma once

#include <vector>

#include "utility_header/id.h"

namespace window_provider
{

// The type representing a window handle
using WindowHandle = utility_header::GenerationID;

// The type representing a collection of window handles
using WindowHandles = std::vector<WindowHandle>;

} // namespace window_provider