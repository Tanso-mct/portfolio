#pragma once

#include "render_graph/include/dll_config.h"
#include "render_graph/include/pipeline.h"

namespace render_graph
{

namespace shadow_composition_pipeline
{

// Root parameter indices
enum class RootParameterIndex : UINT
{
    LIGHT_VIEW_MATRIX,
    LIGHT_PROJ_MATRIX,
    INV_VIEW_PROJ_MATRIX,
    CONFIG,
    SHADOW_MAP,
    DEPTH,
    NORMAL,
    COUNT
};

// Root texture indices
enum class RangeIndex : UINT
{
    SHADOW_MAP,
    DEPTH,
    NORMAL,
    COUNT
};

} // namespace shadow_composition_pipeline

// Pipeline class
class RENDER_GRAPH_DLL ShadowCompositionPipeline :
    public Pipeline
{
public:
    ShadowCompositionPipeline() = default;
    ~ShadowCompositionPipeline() = default;
    bool Setup() override;
    void SetRootParameters(
        ID3D12GraphicsCommandList* command_list, const PassAPI& pass_api) override;
};

} // namespace render_graph