#include "render_graph/src/pch.h"
#include "render_graph/include/shadowing_pipeline.h"

using Microsoft::WRL::ComPtr;

#include "utility_header/logger.h"
#include "directx12_util/include/wrapper.h"
#include "directx12_util/include/d3dx12.h"
#include "directx12_util/include/helper.h"
#include "render_graph/include/shadowing_pass.h"

namespace render_graph
{

bool ShadowingPipeline::Setup()
{
    HRESULT hr = E_FAIL;
    bool result = false;

    // Get device
    dx12_util::Device& device = dx12_util::Device::GetInstance();

    /*******************************************************************************************************************
     * Create root signature
    /******************************************************************************************************************/

    // Feature support check
    D3D12_FEATURE_DATA_ROOT_SIGNATURE feature_data = {};
    feature_data.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
    hr = device.Get()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &feature_data, sizeof(feature_data));
    if (FAILED(hr))
        feature_data.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

    // SamplerState setting
    const UINT SAMPLER_COUNT = 1;
    CD3DX12_STATIC_SAMPLER_DESC sampler
    (
        0, // ShaderRegister
        D3D12_FILTER_MIN_MAG_MIP_LINEAR,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressU
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressV
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressW
        0.0f, // MipLODBias
        0,    // MaxAnisotropy
        D3D12_COMPARISON_FUNC_NEVER,
        D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK,
        0.0f, // MinLOD
        D3D12_FLOAT32_MAX, // MaxLOD
        D3D12_SHADER_VISIBILITY_PIXEL,
        0 // RegisterSpace
    );

    // Rootsignature setting
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_desc;
    CD3DX12_ROOT_PARAMETER1 root_parameters[(UINT)shadowing_pipeline::RootParameterIndex::COUNT];

    // Set constant buffers
    root_parameters[(UINT)shadowing_pipeline::RootParameterIndex::LIGHT_VIEW_MATRIX].InitAsConstantBufferView(
        (UINT)shadowing_pipeline::RootParameterIndex::LIGHT_VIEW_MATRIX, 0, 
        D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE, D3D12_SHADER_VISIBILITY_VERTEX);

    root_parameters[(UINT)shadowing_pipeline::RootParameterIndex::LIGHT_PROJ_MATRIX].InitAsConstantBufferView(
        (UINT)shadowing_pipeline::RootParameterIndex::LIGHT_PROJ_MATRIX, 0, 
        D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE, D3D12_SHADER_VISIBILITY_VERTEX);

    root_parameters[(UINT)shadowing_pipeline::RootParameterIndex::WORLD_MATRIX].InitAsConstantBufferView(
        (UINT)shadowing_pipeline::RootParameterIndex::WORLD_MATRIX, 0, 
        D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE, D3D12_SHADER_VISIBILITY_VERTEX);

    // Initialize the root signature description.
    root_signature_desc.Init_1_1(
        (UINT)shadowing_pipeline::RootParameterIndex::COUNT, root_parameters, SAMPLER_COUNT, &sampler,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    // Integrate parameters to create configuration binaries
    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    hr = D3DX12SerializeVersionedRootSignature(
        &root_signature_desc, feature_data.HighestVersion, signature.GetAddressOf(), error.GetAddressOf());
    if (FAILED(hr))
    {
        utility_header::ConsoleLogErr(
            {"Failed to serialize root signature for LambertPipeline."}, hr,
            __FILE__, __LINE__, __FUNCTION__);
        return false; // Setup failed
    }

    // Create ID3D12RootSignature with the binary created by merging.
    hr = device.Get()->CreateRootSignature(
        0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(rootSignature_.GetAddressOf()));
    if (FAILED(hr))
    {
        utility_header::ConsoleLogErr(
            {"Failed to create root signature for LambertPipeline."}, hr,
            __FILE__, __LINE__, __FUNCTION__);
        return false; // Setup failed
    }

    /*******************************************************************************************************************
     * Create pipeline state object
    /******************************************************************************************************************/

    // Load vertex shader binary
    dx12_util::ShaderBinary vertex_shader_binary;
    result = dx12_util::LoadCSO("../resources/render_graph/data/shadowing_pass_vs.cso", vertex_shader_binary);

    // Load pixel shader binary
    dx12_util::ShaderBinary pixel_shader_binary;
    result = dx12_util::LoadCSO("../resources/render_graph/data/shadowing_pass_ps.cso", pixel_shader_binary);

    // Pipeline state setting
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};

    // Input assembler
    pso_desc.InputLayout = { shadowing_pass::INPUT_LAYOUT, _countof(shadowing_pass::INPUT_LAYOUT) };

    // Root signature
    pso_desc.pRootSignature = rootSignature_.Get();

    // Shaders
    pso_desc.VS.BytecodeLength = vertex_shader_binary.size;
    pso_desc.PS.BytecodeLength = pixel_shader_binary.size;
    pso_desc.VS.pShaderBytecode = vertex_shader_binary.data.get();
    pso_desc.PS.pShaderBytecode = pixel_shader_binary.data.get();

    // Rasterizer state
    pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pso_desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

    // Blend state
    pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

    // Depth stencil state
    pso_desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    pso_desc.DSVFormat = shadowing_pass::SHADOW_MAP_FORMAT;

    // Sampler
    pso_desc.SampleMask = UINT_MAX;
    pso_desc.SampleDesc.Count = 1;

    // Render Target Format
    pso_desc.NumRenderTargets = 0;

    // Primitive topology
    pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    // Create pipeline state object
    hr = device.Get()->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&pipelineState_));
    if (FAILED(hr))
    {
        utility_header::ConsoleLogErr(
            {"Failed to create pipeline state object for LambertPipeline."}, hr,
            __FILE__, __LINE__, __FUNCTION__);
        return false; // Setup failed
    }

    return true; // Setup successful
}

void ShadowingPipeline::SetRootParameters(
    ID3D12GraphicsCommandList* command_list, const PassAPI& pass_api)
{
    // Cast to ShadowingPassAPI
    const ShadowingPassAPI* shadowing_pass_api = dynamic_cast<const ShadowingPassAPI*>(&pass_api);
    assert(shadowing_pass_api != nullptr && "Failed to cast to ShadowingPassAPI.");

    // Set light view-projection matrix buffer
    SetBuffer(
        shadowing_pass_api->GetDrawingLightViewMatrixBufferHandle(), shadowing_pass_api->GetCurrentReadToken(),
        command_list, (UINT)shadowing_pipeline::RootParameterIndex::LIGHT_VIEW_MATRIX);

    // Set light projection matrix buffer
    SetBuffer(
        shadowing_pass_api->GetDrawingLightProjMatrixBufferHandle(), shadowing_pass_api->GetCurrentReadToken(),
        command_list, (UINT)shadowing_pipeline::RootParameterIndex::LIGHT_PROJ_MATRIX);

    // Set world matrix buffer
    SetBuffer(
        shadowing_pass_api->GetDrawingWorldMatrixBufferHandle(), shadowing_pass_api->GetCurrentReadToken(),
        command_list, (UINT)shadowing_pipeline::RootParameterIndex::WORLD_MATRIX);
}

} // namespace render_graph