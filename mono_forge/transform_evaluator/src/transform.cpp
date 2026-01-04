#include "transform_evaluator/src/pch.h"
#include "transform_evaluator/include/transform.h"

using namespace DirectX;

namespace transform_evaluator 
{

transform_evaluator::Transform::Transform(XMFLOAT3 translation, XMFLOAT4 rotation, XMFLOAT3 scale) :
    world_trs_{translation, rotation, scale},
    previous_world_trs_{translation, rotation, scale}
{
}

const TRS& transform_evaluator::Transform::GetWorldTRS() const
{
    return world_trs_;
}

void transform_evaluator::Transform::SetWorldTRS(const TRS& trs)
{
    previous_world_trs_ = world_trs_;
    world_trs_ = trs;
}

const TRS& transform_evaluator::Transform::GetPreviousWorldTRS() const
{
    return previous_world_trs_;
}

const TRS& transform_evaluator::Transform::GetLocalTRS() const
{
    return local_trs_;
}

void transform_evaluator::Transform::SetLocalTRS(const TRS& trs)
{
    previous_local_trs_ = local_trs_;
    local_trs_ = trs;
}

const TRS& transform_evaluator::Transform::GetPreviousLocalTRS() const
{
    return previous_local_trs_;
}

} // namespace transform_evaluator