#pragma once

#include "render_graph/include/dll_config.h"
#include "render_graph/include/pipeline.h"

namespace render_graph
{

namespace phong_pipeline
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
    SPECULAR_TEXTURE,
    ROUGHNESS_TEXTURE,
    METALNESS_TEXTURE,
    EMISSION_TEXTURE,
    COUNT
};

// Root texture indices
enum class RangeIndex : UINT
{
    ALBEDO,
    NORMAL,
    AO,
    SPECULAR,
    ROUGHNESS,
    METALNESS,
    EMISSION,
    COUNT
};

} // namespace phong_pipeline

// Pipeline class
class RENDER_GRAPH_DLL PhongPipeline :
    public Pipeline
{
public:
    PhongPipeline() = default;
    ~PhongPipeline() = default;
    bool Setup() override;
    void SetRootParameters(
        ID3D12GraphicsCommandList* command_list, const PassAPI& pass_api) override;
};

} // namespace render_graph