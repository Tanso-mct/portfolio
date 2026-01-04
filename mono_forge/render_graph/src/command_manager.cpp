#include "render_graph/src/pch.h"
#include "render_graph/include/command_manager.h"

namespace render_graph
{

CommandSet& render_graph::CommandSetManager::GetCommand(const CommandSetHandle* handle)
{
    assert(handle != nullptr && "CommandSetHandle pointer is null.");
    return container_.Get(*handle);
}

CommandSetHandle render_graph::CommandSetAdder::AddCommandSet(std::unique_ptr<CommandSet> command)
{
    CommandSetHandle handle = CommandSetHandle();
    container_.WithUniqueLock([&](CommandSetContainer& container) 
    {
        // Add the command and get its handle
        handle = container.Add(std::move(command));
    });

    return handle;
}

void render_graph::CommandSetEraser::EraseCommandSet(const CommandSetHandle* handle)
{
    assert(handle != nullptr && "CommandSetHandle pointer is null.");

    container_.WithUniqueLock([&](CommandSetContainer& container) 
    {
        // Erase the command
        container.Erase(*handle);
    });
}

} // namespace render_graph