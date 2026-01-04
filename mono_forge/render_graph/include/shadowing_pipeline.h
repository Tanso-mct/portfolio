#pragma once

#include "render_graph/include/dll_config.h"
#include "render_graph/include/pipeline.h"

namespace render_graph
{

namespace shadowing_pipeline
{

// Root parameter indices
enum class RootParameterIndex : UINT
{
    LIGHT_VIEW_MATRIX,
    LIGHT_PROJ_MATRIX,
    WORLD_MATRIX,
    COUNT
};

// Root texture indices
enum class RangeIndex : UINT
{
    COUNT
};

} // namespace shadowing_pipeline

// Pipeline class
class RENDER_GRAPH_DLL ShadowingPipeline :
    public Pipeline
{
public:
    ShadowingPipeline() = default;
    ~ShadowingPipeline() = default;
    bool Setup() override;
    void SetRootParameters(
        ID3D12GraphicsCommandList* command_list, const PassAPI& pass_api) override;
};

} // namespace render_graph