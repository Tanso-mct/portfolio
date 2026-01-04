#include "render_graph/src/pch.h"
#include "render_graph/include/render_pass.h"

namespace render_graph
{

void RenderPassBuilder::Read(const ResourceHandle* handle)
{
    render_pass_.GetReadToken().PermitAccess(handle);
}

void RenderPassBuilder::Write(const ResourceHandle* handle)
{
    render_pass_.GetWriteToken().PermitAccess(handle);
}

bool RenderPass::Setup(RenderPassBuilder &builder)
{
    assert(setup_func_); // Ensure the setup function is valid
    return setup_func_(builder);
}

bool RenderPass::Execute(RenderPassContext &context)
{
    assert(execute_func_); // Ensure the execute function is valid
    return execute_func_(*this, context);
}

} // namespace render_graph