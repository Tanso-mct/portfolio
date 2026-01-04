#pragma once

#include <vector>

#include "utility_header/id.h"

namespace transform_evaluator 
{

// The handle type used to reference Transforms
using TransformHandle = utility_header::GenerationID;

// A collection of TransformHandles
using TransformHandles = std::vector<TransformHandle>;

} // namespace transform_evaluator