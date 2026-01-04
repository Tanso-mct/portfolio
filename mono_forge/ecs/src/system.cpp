#include "ecs/src/pch.h"
#include "ecs/include/system.h"

namespace ecs
{

bool ecs::System::PreUpdate(World& world)
{
    return true;
}

bool ecs::System::Update(World& world)
{
    return true;
}

bool ecs::System::PostUpdate(World& world)
{
    return true;
}

} // namespace ecs