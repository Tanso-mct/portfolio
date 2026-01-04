#pragma once

#include <vector>
#include <memory>
#include <queue>
#include <functional>

#include "class_template/non_copy.h"
#include "class_template/thread_safer.h"

#include "mono_service/include/dll_config.h"

namespace mono_service
{

// The service api interface
class ServiceAPI
{
public:
    ServiceAPI() = default;
    virtual ~ServiceAPI() = default;
};

// The abstract base class for a service command
class ServiceCommand
{
public:
    ServiceCommand() = default;
    virtual ~ServiceCommand() = default;

    // Execute the service command
    virtual bool Execute(ServiceAPI& service_api) = 0;
};

// The template class for a service command implementation
// It takes a callable object (e.g., lambda, function pointer, functor) as a parameter
template <typename Func>
class ServiceCommandImpl : public ServiceCommand
{
public:
    ServiceCommandImpl(Func&& func) : func_(std::move(func)) {}
    virtual ~ServiceCommandImpl() = default;

    // Execute the service command
    virtual bool Execute(ServiceAPI& service_api) override
    {
        return func_(service_api);
    }

private:
    Func func_;
};

// The class representing a list of service commands
// It holds multiple service commands to be executed in sequence
// You can create derived classes to implement specific command lists and add commands from the derived classes
class MONO_SERVICE_DLL ServiceCommandList :
    public class_template::NonCopyable
{
public:
    ServiceCommandList() = default;
    virtual ~ServiceCommandList() = default;

    // Get the list of service commands
    const std::vector<std::unique_ptr<ServiceCommand>>& GetCommands() const;

protected:
    // Add a service command to the list
    template <typename Func>
    void AddCommand(Func func)
    {
        using Decayed = std::decay_t<Func>;
        using CommandType = ServiceCommandImpl<Decayed>;

        // Add the command to the list
        commands_.emplace_back(std::make_unique<CommandType>(std::move(func)));
    }

private:
    std::vector<std::unique_ptr<ServiceCommand>> commands_;
};

// The class representing a service command queue
// It holds and manages service command lists
class MONO_SERVICE_DLL ServiceCommandQueue :
    public class_template::NonCopyable
{
public:
    ServiceCommandQueue() = default;
    virtual ~ServiceCommandQueue() = default;

    // Enqueue a service command list
    void EnqueueCommandList(std::unique_ptr<ServiceCommandList> command_list);

    // Dequeue a service command list
    std::unique_ptr<ServiceCommandList> DequeueCommandList();

    // Check if the queue is empty
    bool IsEmpty() const;

private:
    // Queue of service command lists
    std::queue<std::unique_ptr<ServiceCommandList>> command_lists_;
};

} // namespace mono_service