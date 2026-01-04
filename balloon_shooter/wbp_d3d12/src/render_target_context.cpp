#include "wbp_d3d12/src/pch.h"
#include "wbp_d3d12/include/render_target_context.h"

void wbp_d3d12::RenderTargetContext::Resize()
{
    // Resize vectors to hold the specified number of render targets and depth stencils
    renderTargets_.resize(renderTargetCount_);
    commandAllocators_.resize(renderTargetCount_);
    commandLists_.resize(renderTargetCount_);
    depthStencils_.resize(depthStencilCount_);
}