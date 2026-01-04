#include "wbp_d3d12/src/pch.h"
#include "wbp_d3d12/include/fence_context.h"

#include "wbp_d3d12/include/d3d12_helpers.h"

wbp_d3d12::FenceContext::~FenceContext()
{
    wbp_d3d12::CloseFenceEvent(fenceEvent_);
}

void wbp_d3d12::FenceContext::Resize()
{
    fenceValues_.resize(fenceCount_, INITIAL_FENCE_VALUE);
}