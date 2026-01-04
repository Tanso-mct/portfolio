#pragma once

#include "class_template/singleton.h"
#include "utility_header/container.h"
#include "directx12_util/include/wrapper.h"

#include "render_graph/include/dll_config.h"
#include "render_graph/include/command_handle.h"
#include "render_graph/include/command_set.h"

namespace render_graph
{

// The container type used to contain commands
using CommandSetContainer = utility_header::Container<CommandSet>;

// The command manager class that manages commands and their handles
class RENDER_GRAPH_DLL CommandSetManager :
    public class_template::Singleton<CommandSetManager>
{
public:
    CommandSetManager(CommandSetContainer& container) : container_(container) {}
    ~CommandSetManager() override = default;

    // Execute a function with a unique lock on the command container
    template <typename Func>
    void WithLock(Func&& func) 
    {
        container_.WithUniqueLock([&](CommandSetContainer& container) 
        {
            func(*this);
        });
    }

    // Get a command
    // It must be called in WithLock lambda function
    CommandSet& GetCommand(const CommandSetHandle* handle);

private:
    CommandSetContainer& container_;
};

// The command adder class that adds commands to the command container
class RENDER_GRAPH_DLL CommandSetAdder
{
public:
    CommandSetAdder(CommandSetContainer& container) : container_(container) {}
    ~CommandSetAdder() = default;

    // Add a command and return its handle
    CommandSetHandle AddCommandSet(std::unique_ptr<CommandSet> command);

private:
    CommandSetContainer& container_;
};

// The command eraser class that erases commands from the command container
class RENDER_GRAPH_DLL CommandSetEraser
{
public:
    CommandSetEraser(CommandSetContainer& container) : container_(container) {}
    ~CommandSetEraser() = default;

    // Erase a command given its handle
    void EraseCommandSet(const CommandSetHandle* handle);

private:
    CommandSetContainer& container_;
};

} // namespace render_graph