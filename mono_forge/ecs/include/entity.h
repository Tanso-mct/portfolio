#pragma once

#include <vector>

#include "utility_header/id.h"

namespace ecs
{

// An Entity is represented by an GenerationID
// Entity only contains an index and a generation
using Entity = utility_header::GenerationID;

// The staging area for entities
// It use when you want to create entities at multi threads and commit them to the world at once
using StagingEntityArea = std::vector<Entity>;

} // namespace ecs