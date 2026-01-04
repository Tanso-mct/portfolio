#include "render_graph/src/pch.h"
#include "render_graph/include/command_set.h"

namespace render_graph
{

CommandSet::CommandSet(D3D12_COMMAND_LIST_TYPE type) :
    type_(type)
{
}

bool CommandSet::Setup()
{
    // Create command allocator
    command_allocator_ = dx12_util::CommandAllocator::CreateInstance<dx12_util::CommandAllocator>(
        type_, dx12_util::Device::GetInstance().Get());
    if (!command_allocator_)
        return false; // Failed to create command allocator

    // Create command list
    command_list_ = dx12_util::CommandList::CreateInstance<dx12_util::CommandList>(
        *command_allocator_, dx12_util::Device::GetInstance().Get());
    if (!command_list_)
        return false; // Failed to create command list

    return true;
}

dx12_util::CommandList& CommandSet::GetCommandList()
{
    assert(command_list_ != nullptr && "Command list is not initialized");
    return *command_list_;
}

dx12_util::CommandAllocator& CommandSet::GetCommandAllocator()
{
    assert(command_allocator_ != nullptr && "Command allocator is not initialized");
    return *command_allocator_;
}

void render_graph::CommandSet::ResetCommand()
{
    HRESULT hr = E_FAIL;

    // Reset command allocator
    hr = command_allocator_->Get()->Reset();
    assert(SUCCEEDED(hr) && "Failed to reset command allocator.");

    // Reset command list
    hr = command_list_->Get()->Reset(command_allocator_->Get(), nullptr);
    assert(SUCCEEDED(hr) && "Failed to reset command list.");
}

void render_graph::CommandSet::CloseCommand()
{
    HRESULT hr = E_FAIL;

    // Close command list
    hr = command_list_->Get()->Close();
    assert(SUCCEEDED(hr) && "Failed to close command list.");
}

} // namespace render_graph