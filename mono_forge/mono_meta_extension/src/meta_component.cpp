#include "mono_meta_extension/src/pch.h"
#include "mono_meta_extension/include/meta_component.h"

namespace mono_meta_extension
{

MetaComponent::MetaComponent()
{
}

MetaComponent::~MetaComponent()
{
}

bool MetaComponent::Setup(ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    MetaComponent::SetupParam* metaComponent_param
        = dynamic_cast<MetaComponent::SetupParam*>(&param);
    assert(metaComponent_param != nullptr && "Invalid setup param type for MetaComponent");

    // Set parameters
    name_ = metaComponent_param->name;
    active_self_ = metaComponent_param->active_self;
    tag_ = metaComponent_param->tag;
    layer_ = metaComponent_param->layer;

    return true; // Success
}

bool MetaComponent::Apply(const ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    const MetaComponent::SetupParam* metaComponent_param
        = dynamic_cast<const MetaComponent::SetupParam*>(&param);
    assert(metaComponent_param != nullptr && "Invalid setup param type for MetaComponent");

    // Apply changes
    name_ = metaComponent_param->name;
    active_self_ = metaComponent_param->active_self;
    tag_ = metaComponent_param->tag;
    layer_ = metaComponent_param->layer;

    return true; // Success
}

ecs::ComponentID MetaComponent::GetID() const
{
    return MetaComponentHandle::ID();
}

} // namespace mono_meta_extension