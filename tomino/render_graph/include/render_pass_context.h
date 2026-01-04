#pragma once

#include "class_template/non_copy.h"
#include "class_template/instance.h"
#include "directx12_util/include/wrapper.h"

#include "render_graph/include/dll_config.h"

namespace render_graph
{

// Context provided to a RenderPass during execution
class RENDER_GRAPH_DLL RenderPassContext :
    public class_template::NonCopyable
{
public:
    RenderPassContext(dx12_util::CommandList& command_list);
    virtual ~RenderPassContext();

    // Get the command list for this render pass
    dx12_util::CommandList& GetCommandList();

private:
    // The command list for this render pass
    dx12_util::CommandList& command_list_;
};

} // namespace render_graph