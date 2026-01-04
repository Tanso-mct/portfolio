#pragma once

#include <cassert>
#include <memory>

#include "class_template/argument.h"
#include "class_template/non_copy.h"

namespace class_template
{

template <typename Interface, typename ConstructArgs, typename SetupArgs>
class InstanceSetuper;

template <typename Interface, typename... ConstructArgs, typename... SetupArgs>
class InstanceSetuper<Interface, ConstructArgList<ConstructArgs...>, SetupArgList<SetupArgs...>>
{
public:
    InstanceSetuper() = default;
    virtual ~InstanceSetuper() = default;

    // Create an instance of T
    // Returns nullptr if creation fails
    template <typename Concrete>
    static std::unique_ptr<Interface> CreateInstance(ConstructArgs... construct_args, SetupArgs... setup_args)
    {
        // Instantiate T
        std::unique_ptr<Concrete> instance = std::make_unique<Concrete>(std::forward<ConstructArgs>(construct_args)...);
        assert(instance != nullptr); // Ensure instantiation succeeded because constructor should not fail

        // Cast to InstanceSetuper
        InstanceSetuper<Interface, ConstructArgList<ConstructArgs...>, SetupArgList<SetupArgs...>>* setuper
            = dynamic_cast<InstanceSetuper<Interface, ConstructArgList<ConstructArgs...>, SetupArgList<SetupArgs...>>*>(instance.get());
        assert(setuper != nullptr); // Ensure the cast succeeded

        // Call pre-setup
        if (!setuper->PreSetup())
            return nullptr; // If pre-setup fails, return nullptr

        // Create the instance
        if (!setuper->Setup(std::forward<SetupArgs>(setup_args)...))
            return nullptr; // If creation fails, return nullptr

        // Call post-setup
        if (!setuper->PostCreate())
            return nullptr; // If post-setup fails, return nullptr

        return instance;
    }

    virtual bool PreSetup()
    {
        // No special action needed before creation by default
        return true;
    }

    // Create the instance
    virtual bool Setup(SetupArgs... args)
    {
        // No special creation needed by default
        return true;
    }

    // Call after creating the instance
    virtual bool PostCreate()
    {
        // No special action needed after creation by default
        return true;
    }
};

template <typename Interface, typename ConstructArgs, typename SetupArgs>
class InstanceGuard;

template <typename Interface, typename... ConstructArgs, typename... SetupArgs>
class InstanceGuard<Interface, ConstructArgList<ConstructArgs...>, SetupArgList<SetupArgs...>> :
    public InstanceSetuper<Interface, ConstructArgList<ConstructArgs...>, SetupArgList<SetupArgs...>>,
    public NonCopyable
{
public:
    InstanceGuard() = default;
    virtual ~InstanceGuard() = default;

    bool PreSetup() override final
    {
        return true;
    }

    bool PostCreate() override final
    {
        assert(!setup_flag); // Ensure the instance is not already setup
        setup_flag = true; // Mark as setup
        return true;
    }

    // Check if the instance is setup
    bool IsSetup() const { return setup_flag; }

private:
    bool setup_flag = false; // Indicates whether the instance is setup
};

} // namespace class_template