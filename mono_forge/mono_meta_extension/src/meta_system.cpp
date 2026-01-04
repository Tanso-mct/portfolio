#include "mono_meta_extension/src/pch.h"
#include "mono_meta_extension/include/meta_system.h"

namespace mono_meta_extension
{

MetaSystem::MetaSystem()
{
    // Create singleton tag generator
    tag_generator_ = std::make_unique<MetaTagGenerator>();

    // Create singleton layer generator
    layer_generator_ = std::make_unique<MetaLayerGenerator>();
}

MetaSystem::~MetaSystem()
{
    // Cleanup generators
    tag_generator_.reset();
    layer_generator_.reset();
}

bool MetaSystem::PreUpdate(ecs::World& world)
{
    return true; // Success
}

bool MetaSystem::Update(ecs::World& world)
{
    return true; // Success
}

bool MetaSystem::PostUpdate(ecs::World& world)
{
    return true; // Success
}

ecs::SystemID MetaSystem::GetID() const
{
    return MetaSystemHandle::ID();
}

} // namespace mono_meta_extension