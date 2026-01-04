#pragma once

#include "render_graph/include/dll_config.h"
#include "render_graph/include/pipeline.h"

namespace render_graph
{

namespace lambert_pipeline
{

// Root parameter indices
enum class RootParameterIndex : UINT
{
    VIEW_PROJ_MATRIX,
    WORLD_MATRIX,
    MATERIAL,
    ALBEDO_TEXTURE,
    NORMAL_TEXTURE,
    AO_TEXTURE,
    EMISSION_TEXTURE,
    COUNT
};

// Root texture indices
enum class RangeIndex : UINT
{
    ALBEDO,
    NORMAL,
    AO,
    EMISSION,
    COUNT
};

} // namespace lambert_pipeline

// Lambert shading pipeline
class RENDER_GRAPH_DLL LambertPipeline : 
    public Pipeline
{
public:
    LambertPipeline() = default;
    ~LambertPipeline() = default;
    bool Setup() override;
    void SetRootParameters(
        ID3D12GraphicsCommandList* command_list, const PassAPI& pass_api) override;
};

} // namespace render_graph