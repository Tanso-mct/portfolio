#include "render_graph/src/pch.h"
#include "render_graph/include/render_pass_context.h"

namespace render_graph
{

RenderPassContext::RenderPassContext(dx12_util::CommandList& command_list) :
    command_list_(command_list)
{
}

RenderPassContext::~RenderPassContext()
{
}

dx12_util::CommandList& RenderPassContext::GetCommandList()
{
    return command_list_;
}

} // namespace render_graph