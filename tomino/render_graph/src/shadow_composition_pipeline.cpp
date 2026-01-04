#include "render_graph/src/pch.h"
#include "render_graph/include/shadow_composition_pipeline.h"

using Microsoft::WRL::ComPtr;

#include "utility_header/logger.h"
#include "directx12_util/include/wrapper.h"
#include "directx12_util/include/d3dx12.h"
#include "directx12_util/include/helper.h"
#include "render_graph/include/shadow_composition_pass.h"

namespace render_graph
{

bool ShadowCompositionPipeline::Setup()
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
    const UINT SAMPLER_COUNT = 2;
    D3D12_STATIC_SAMPLER_DESC samplers[SAMPLER_COUNT];

    // linear sampler
    samplers[0] = CD3DX12_STATIC_SAMPLER_DESC
    (
        0, // ShaderRegister
        D3D12_FILTER_MIN_MAG_MIP_POINT,
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

    // Compare sampler
    samplers[1] = CD3DX12_STATIC_SAMPLER_DESC
    (
        1, // ShaderRegister
        D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
        D3D12_TEXTURE_ADDRESS_MODE_BORDER, // AddressU
        D3D12_TEXTURE_ADDRESS_MODE_BORDER, // AddressV
        D3D12_TEXTURE_ADDRESS_MODE_BORDER, // AddressW
        0.0f, // MipLODBias
        0,    // MaxAnisotropy
        D3D12_COMPARISON_FUNC_LESS_EQUAL,
        D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, // or TRANSPARENT_WHITE
        0.0f, // MinLOD
        D3D12_FLOAT32_MAX, // MaxLOD
        D3D12_SHADER_VISIBILITY_PIXEL,
        0 // RegisterSpace
    );

    // Rootsignature setting
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_desc;
    CD3DX12_ROOT_PARAMETER1 root_params[(UINT)shadow_composition_pipeline::RootParameterIndex::COUNT];

    // Set constant buffers
    root_params[(UINT)shadow_composition_pipeline::RootParameterIndex::LIGHT_VIEW_MATRIX].InitAsConstantBufferView(
        (UINT)shadow_composition_pipeline::RootParameterIndex::LIGHT_VIEW_MATRIX, 0, 
        D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE, D3D12_SHADER_VISIBILITY_PIXEL);

    root_params[(UINT)shadow_composition_pipeline::RootParameterIndex::LIGHT_PROJ_MATRIX].InitAsConstantBufferView(
        (UINT)shadow_composition_pipeline::RootParameterIndex::LIGHT_PROJ_MATRIX, 0, 
        D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE, D3D12_SHADER_VISIBILITY_PIXEL);

    root_params[(UINT)shadow_composition_pipeline::RootParameterIndex::INV_VIEW_PROJ_MATRIX].InitAsConstantBufferView(
        (UINT)shadow_composition_pipeline::RootParameterIndex::INV_VIEW_PROJ_MATRIX, 0, 
        D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE, D3D12_SHADER_VISIBILITY_PIXEL);

    root_params[(UINT)shadow_composition_pipeline::RootParameterIndex::CONFIG].InitAsConstantBufferView(
        (UINT)shadow_composition_pipeline::RootParameterIndex::CONFIG, 0, 
        D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE, D3D12_SHADER_VISIBILITY_PIXEL);

    // Creating RANGE for textures
    CD3DX12_DESCRIPTOR_RANGE1 ranges[(UINT)shadow_composition_pipeline::RangeIndex::COUNT];

    // Set SRVs for textures
    ranges[(UINT)shadow_composition_pipeline::RangeIndex::SHADOW_MAP].Init(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)shadow_composition_pipeline::RangeIndex::SHADOW_MAP, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

    ranges[(UINT)shadow_composition_pipeline::RangeIndex::DEPTH].Init(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)shadow_composition_pipeline::RangeIndex::DEPTH, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);
    
    ranges[(UINT)shadow_composition_pipeline::RangeIndex::NORMAL].Init(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)shadow_composition_pipeline::RangeIndex::NORMAL, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

    // Set descriptor tables for textures
    root_params[(UINT)shadow_composition_pipeline::RootParameterIndex::SHADOW_MAP].InitAsDescriptorTable(
        1, &ranges[(UINT)shadow_composition_pipeline::RangeIndex::SHADOW_MAP], D3D12_SHADER_VISIBILITY_PIXEL);

    root_params[(UINT)shadow_composition_pipeline::RootParameterIndex::DEPTH].InitAsDescriptorTable(
        1, &ranges[(UINT)shadow_composition_pipeline::RangeIndex::DEPTH], D3D12_SHADER_VISIBILITY_PIXEL);

    root_params[(UINT)shadow_composition_pipeline::RootParameterIndex::NORMAL].InitAsDescriptorTable(
        1, &ranges[(UINT)shadow_composition_pipeline::RangeIndex::NORMAL], D3D12_SHADER_VISIBILITY_PIXEL);

    // Initialize the root signature description.
    root_signature_desc.Init_1_1(
        (UINT)shadow_composition_pipeline::RootParameterIndex::COUNT, root_params, SAMPLER_COUNT, samplers,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    // Integrate parameters to create configuration binaries
    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    hr = D3DX12SerializeVersionedRootSignature(
        &root_signature_desc, feature_data.HighestVersion, signature.GetAddressOf(), error.GetAddressOf());
    if (FAILED(hr))
    {
        utility_header::ConsoleLogErr(
            {"Failed to serialize root signature for LightingPipeline."}, hr,
            __FILE__, __LINE__, __FUNCTION__);
        return false; // Setup failed
    }

    // Create ID3D12RootSignature with the binary created by merging.
    hr = device.Get()->CreateRootSignature(
        0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(rootSignature_.GetAddressOf()));
    if (FAILED(hr))
    {
        utility_header::ConsoleLogErr(
            {"Failed to create root signature for LightingPipeline."}, hr,
            __FILE__, __LINE__, __FUNCTION__);
        return false; // Setup failed
    }

    /*******************************************************************************************************************
     * Create pipeline state object
    /******************************************************************************************************************/

    // Load vertex shader binary
    dx12_util::ShaderBinary vertex_shader_binary;
    result = dx12_util::LoadCSO("../resources/render_graph/data/full_screen_vs.cso", vertex_shader_binary);

    // Load pixel shader binary
    dx12_util::ShaderBinary pixel_shader_binary;
    result = dx12_util::LoadCSO("../resources/render_graph/data/shadow_composition_pass_ps.cso", pixel_shader_binary);

    // Pipeline state setting
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};

    // Input assembler
    pso_desc.InputLayout = { shadow_composition_pass::INPUT_LAYOUT, _countof(shadow_composition_pass::INPUT_LAYOUT) };

    // Root signature
    pso_desc.pRootSignature = rootSignature_.Get();

    // Shaders
    pso_desc.VS.BytecodeLength = vertex_shader_binary.size;
    pso_desc.PS.BytecodeLength = pixel_shader_binary.size;
    pso_desc.VS.pShaderBytecode = vertex_shader_binary.data.get();
    pso_desc.PS.pShaderBytecode = pixel_shader_binary.data.get();

    // Rasterizer state
    pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

    // Blend state
    pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
    {
        D3D12_RENDER_TARGET_BLEND_DESC& rt = pso_desc.BlendState.RenderTarget[i];
        rt.BlendEnable = TRUE;
        rt.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

        // RGB: result = min(Src, Dest)
        rt.SrcBlend = D3D12_BLEND_ONE;
        rt.DestBlend = D3D12_BLEND_ONE;
        rt.BlendOp = D3D12_BLEND_OP_MIN;

        // A: result = min(Src, Dest)
        rt.SrcBlendAlpha = D3D12_BLEND_ONE;
        rt.DestBlendAlpha = D3D12_BLEND_ONE;
        rt.BlendOpAlpha = D3D12_BLEND_OP_MIN;
    }

    // Depth stencil state
    pso_desc.DepthStencilState.DepthEnable = FALSE; // Disable depth for composition pass
    pso_desc.DepthStencilState.StencilEnable = FALSE; // Disable stencil for composition pass
    pso_desc.DSVFormat = DXGI_FORMAT_UNKNOWN; // No depth-stencil format needed

    // Sampler
    pso_desc.SampleMask = UINT_MAX;
    pso_desc.SampleDesc.Count = 1;

    // Render Target Format
    pso_desc.NumRenderTargets = (uint32_t)shadow_composition_pass::RenderTargetIndex::COUNT;
    for (uint32_t i = 0; i < (uint32_t)shadow_composition_pass::RenderTargetIndex::COUNT; ++i)
        pso_desc.RTVFormats[i] = shadow_composition_pass::RENDER_TARGET_FORMATS[i];

    // Primitive topology
    pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    // Create pipeline state object
    hr = device.Get()->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&pipelineState_));
    if (FAILED(hr))
    {
        utility_header::ConsoleLogErr(
            {"Failed to create pipeline state object for LightingPipeline."}, hr,
            __FILE__, __LINE__, __FUNCTION__);
        return false; // Setup failed
    }

    return true; // Setup successful
}

void ShadowCompositionPipeline::SetRootParameters(
    ID3D12GraphicsCommandList* command_list, const PassAPI& pass_api)
{
    // Cast to ShadowCompositionPassAPI
    const ShadowCompositionPassAPI* shadow_composition_pass_api =
        dynamic_cast<const ShadowCompositionPassAPI*>(&pass_api);
    assert(shadow_composition_pass_api != nullptr && "Invalid PassAPI type for ShadowCompositionPipeline");

    // Set light view matrix buffer
    SetBuffer(
        shadow_composition_pass_api->GetDrawingLightViewMatrixBufferHandle(),
        shadow_composition_pass_api->GetReadAccessToken(),
        command_list, (UINT)shadow_composition_pipeline::RootParameterIndex::LIGHT_VIEW_MATRIX);

    // Set light projection matrix buffer
    SetBuffer(
        shadow_composition_pass_api->GetDrawingLightProjMatrixBufferHandle(),
        shadow_composition_pass_api->GetReadAccessToken(),
        command_list, (UINT)shadow_composition_pipeline::RootParameterIndex::LIGHT_PROJ_MATRIX);

    // Set inverse view-projection matrix buffer
    SetBuffer(
        shadow_composition_pass_api->GetCameraInvViewProjMatrixBufferHandle(),
        shadow_composition_pass_api->GetReadAccessToken(),
        command_list, (UINT)shadow_composition_pipeline::RootParameterIndex::INV_VIEW_PROJ_MATRIX);

    // Set shadow composition config buffer
    SetBuffer(
        shadow_composition_pass_api->GetShadowCompositionConfigBufferHandle(),
        shadow_composition_pass_api->GetReadAccessToken(),
        command_list, (UINT)shadow_composition_pipeline::RootParameterIndex::CONFIG);

    // Set shadow map texture
    SetTexture2D(
        shadow_composition_pass_api->GetShadowMapTextureHandle(),
        shadow_composition_pass_api->GetReadAccessToken(),
        command_list, (UINT)shadow_composition_pipeline::RootParameterIndex::SHADOW_MAP);

    // Set depth texture
    SetTexture2D(
        shadow_composition_pass_api->GetDepthTextureHandle(),
        shadow_composition_pass_api->GetReadAccessToken(),
        command_list, (UINT)shadow_composition_pipeline::RootParameterIndex::DEPTH);

    // Set normal texture
    SetTexture2D(
        shadow_composition_pass_api->GetNormalTextureHandle(),
        shadow_composition_pass_api->GetReadAccessToken(),
        command_list, (UINT)shadow_composition_pipeline::RootParameterIndex::NORMAL);
}

} // namespace render_graph