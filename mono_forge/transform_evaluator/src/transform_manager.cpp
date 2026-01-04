#include "transform_evaluator/src/pch.h"
#include "transform_evaluator/include/transform_manager.h"

namespace transform_evaluator 
{

Transform& TransformManager::GetTransform(const TransformHandle& handle)
{
    return container_.Get(handle);
}

const Transform& TransformManager::GetTransform(const TransformHandle& handle) const
{
    return container_.Get(handle);
}

TransformHandle TransformAdder::AddTransform(std::unique_ptr<Transform> transform)
{
    TransformHandle handle;
    container_.WithUniqueLock([&](TransformContainer& container) 
    {
        // Add the transform and get its handle
        handle = container.Add(std::move(transform));
    });

    return handle;
}

void TransformEraser::EraseTransform(const TransformHandle& handle)
{
    container_.WithUniqueLock([&](TransformContainer& container) 
    {
        container.Erase(handle);
    });
}

} // namespace transform_evaluator