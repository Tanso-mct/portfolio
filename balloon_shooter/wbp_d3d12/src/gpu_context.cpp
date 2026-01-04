#include "wbp_d3d12/src/pch.h"
#include "wbp_d3d12/include/gpu_context.h"

wbp_d3d12::GPUContext &wbp_d3d12::GPUContext::GetInstance()
{
    static GPUContext instance;
    return instance;
}