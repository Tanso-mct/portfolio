#pragma once

#include <d3d12.h>
#include <dxgi.h>
#include <DirectXMath.h>

#include "render_graph/include/dll_config.h"
#include "render_graph/include/pipeline.h"

namespace render_graph
{

namespace lighting_pipeline
{

// Root parameter indices
enum class RootParameterIndex : UINT
{
    INV_VIEW_PROJ_MATRIX,
    LIGHT_CONFIG_BUFFER,
    LIGHTS,
    ALBEDO,
    NORMAL,
    METALNESS,
    ROUGHNESS,
    SPECULAR,
    AO,
    EMISSION,
    MASK_MATERIAL,
    MASK_SHADOW,
    DEPTH,
    COUNT
};

// Root texture indices
enum class RangeIndex : UINT
{
    LIGHTS,
    ALBEDO,
    NORMAL,
    METALNESS,
    ROUGHNESS,
    SPECULAR,
    AO,
    EMISSION,
    MASK_MATERIAL,
    MASK_SHADOW,
    DEPTH,
    COUNT
};

} // namespace lighting_pipeline

// Lighting shader pipeline
class RENDER_GRAPH_DLL LightingPipeline : 
    public Pipeline
{
public:
    LightingPipeline() = default;
    ~LightingPipeline() = default;
    bool Setup() override;
    void SetRootParameters(
        ID3D12GraphicsCommandList* command_list, const PassAPI& pass_api) override;
};

} // namespace render_graph