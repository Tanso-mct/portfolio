#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

#include "class_template/non_copy.h"
#include "class_template/instance.h"
#include "utility_header/id.h"

#include "render_graph/include/dll_config.h"
#include "render_graph/include/resource_manager.h"

namespace render_graph
{

// The command class that wraps command allocator and command list
class RENDER_GRAPH_DLL CommandSet :
    public class_template::NonCopyable,
    public class_template::InstanceGuard<
        CommandSet,
        class_template::ConstructArgList<D3D12_COMMAND_LIST_TYPE>,
        class_template::SetupArgList<>>
{
public:
    CommandSet(D3D12_COMMAND_LIST_TYPE type);
    ~CommandSet() override = default;
    virtual bool Setup() override;

    // Get the command list
    dx12_util::CommandList& GetCommandList();

    // Get the command allocator
    dx12_util::CommandAllocator& GetCommandAllocator();

    // Reset the command allocator and command list
    void ResetCommand();

    // Close the command list
    void CloseCommand();

private:
    // The type of the command list
    const D3D12_COMMAND_LIST_TYPE type_;
    
    // The command allocator
    std::unique_ptr<dx12_util::CommandAllocator> command_allocator_ = nullptr;

    // The command list
    std::unique_ptr<dx12_util::CommandList> command_list_ = nullptr;
};

} // namespace render_graph