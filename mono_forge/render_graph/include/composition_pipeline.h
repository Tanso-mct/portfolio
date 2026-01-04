#pragma once

#include "render_graph/include/dll_config.h"
#include "render_graph/include/pipeline.h"

namespace render_graph
{

namespace composition_pipeline
{

// Root parameter indices
enum class RootParameterIndex : UINT
{
    POST_PROCESS_TEXTURE,
    UI_TEXTURE,
    COUNT
};

// Root texture indices
enum class RangeIndex : UINT
{
    POST_PROCESS_TEXTURE,
    UI_TEXTURE,
    COUNT
};

} // namespace composition_pipeline

// Composition shader pipeline
class RENDER_GRAPH_DLL CompositionPipeline :
    public Pipeline
{
public:
    CompositionPipeline() = default;
    ~CompositionPipeline() = default;
    bool Setup() override;
    void SetRootParameters(
        ID3D12GraphicsCommandList* command_list, const PassAPI& pass_api) override;
};

} // namespace render_graph