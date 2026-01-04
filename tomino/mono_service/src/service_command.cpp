#include "mono_service/src/pch.h"
#include "mono_service/include/service_command.h"

namespace mono_service
{

const std::vector<std::unique_ptr<ServiceCommand>>& ServiceCommandList::GetCommands() const
{
    return commands_;
}

void ServiceCommandQueue::EnqueueCommandList(std::unique_ptr<ServiceCommandList> command_list)
{
    // Add the command list to the queue
    command_lists_.emplace(std::move(command_list));
}

std::unique_ptr<ServiceCommandList> ServiceCommandQueue::DequeueCommandList()
{
    assert(!command_lists_.empty() && "Attempted to dequeue from an empty command queue.");

    // Get the front command list
    std::unique_ptr<ServiceCommandList> command_list = std::move(command_lists_.front());

    // Remove the front command list from the queue
    command_lists_.pop();

    return command_list; // Return the dequeued command list
}

bool ServiceCommandQueue::IsEmpty() const
{
    return command_lists_.empty();
}

} // namespace mono_service