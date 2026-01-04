#pragma once

#include "class_template/singleton.h"
#include "utility_header/container.h"

#include "transform_evaluator/include/dll_config.h"
#include "transform_evaluator/include/transform.h"
#include "transform_evaluator/include/transform_handle.h"

namespace transform_evaluator 
{

// The container type used to contain Transforms
using TransformContainer = utility_header::Container<Transform>;

// The TransformManager class that manages Transforms and their Handles
class TRANSFORM_EVALUATOR_DLL TransformManager :
    public class_template::Singleton<TransformManager>
{
public:
    TransformManager(TransformContainer& container) : container_(container) {}
    ~TransformManager() override = default;

    // Execute a function with a unique lock on the transform container
    template <typename Func>
    void WithLock(Func&& func) 
    {
        container_.WithUniqueLock([&](TransformContainer& container) 
        {
            func(*this);
        });
    }

    // Get a Transform for writing using its Handle
    // It must be called in WithLock lambda function
    Transform& GetTransform(const TransformHandle& handle);

    // Get a Transform for reading using its Handle
    // It must be called in WithLock lambda function
    const Transform& GetTransform(const TransformHandle& handle) const;

private:
    TransformContainer& container_;
};

// The TransformAdder class that adds transform to the container
class TRANSFORM_EVALUATOR_DLL TransformAdder
{
public:
    TransformAdder(TransformContainer& container) : container_(container) {}
    virtual ~TransformAdder() = default;

    // Add a new Transform and return its Handle
    TransformHandle AddTransform(std::unique_ptr<Transform> transform);

private:
    TransformContainer& container_;
};

// The TransformEraser class that erases transform from the container
class TRANSFORM_EVALUATOR_DLL TransformEraser
{
public:
    TransformEraser(TransformContainer& container) : container_(container) {}
    virtual ~TransformEraser() = default;

    // Erase a Transform using its Handle
    void EraseTransform(const TransformHandle& handle);

private:
    TransformContainer& container_;
};

} // namespace transform_evaluator