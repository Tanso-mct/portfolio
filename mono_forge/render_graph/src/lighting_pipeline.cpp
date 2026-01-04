#include "render_graph/src/pch.h"
#include "render_graph/include/lighting_pipeline.h"

using Microsoft::WRL::ComPtr;

#include "utility_header/logger.h"
#include "directx12_util/include/wrapper.h"
#include "directx12_util/include/d3dx12.h"
#include "directx12_util/include/helper.h"

#include "render_graph/include/lighting_pass.h"

namespace render_graph
{

bool LightingPipeline::Setup()
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
    CD3DX12_ROOT_PARAMETER1 root_paras[(UINT)lighting_pipeline::RootParameterIndex::COUNT];

    // Set constant buffers
    root_paras[(UINT)lighting_pipeline::RootParameterIndex::INV_VIEW_PROJ_MATRIX].InitAsConstantBufferView(
        (UINT)lighting_pipeline::RootParameterIndex::INV_VIEW_PROJ_MATRIX, 0, 
        D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE, D3D12_SHADER_VISIBILITY_PIXEL);

    root_paras[(UINT)lighting_pipeline::RootParameterIndex::LIGHT_CONFIG_BUFFER].InitAsConstantBufferView(
        (UINT)lighting_pipeline::RootParameterIndex::LIGHT_CONFIG_BUFFER, 0,
        D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE, D3D12_SHADER_VISIBILITY_PIXEL);

    // Creating RANGE for textures
    CD3DX12_DESCRIPTOR_RANGE1 ranges[(UINT)lighting_pipeline::RangeIndex::COUNT];

    // Set SRVs for textures
    ranges[(UINT)lighting_pipeline::RangeIndex::LIGHTS].Init(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)lighting_pipeline::RangeIndex::LIGHTS, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

    ranges[(UINT)lighting_pipeline::RangeIndex::ALBEDO].Init(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)lighting_pipeline::RangeIndex::ALBEDO, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

    ranges[(UINT)lighting_pipeline::RangeIndex::NORMAL].Init(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)lighting_pipeline::RangeIndex::NORMAL, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

    ranges[(UINT)lighting_pipeline::RangeIndex::METALNESS].Init(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)lighting_pipeline::RangeIndex::METALNESS, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

    ranges[(UINT)lighting_pipeline::RangeIndex::ROUGHNESS].Init(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)lighting_pipeline::RangeIndex::ROUGHNESS, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

    ranges[(UINT)lighting_pipeline::RangeIndex::SPECULAR].Init(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)lighting_pipeline::RangeIndex::SPECULAR, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

    ranges[(UINT)lighting_pipeline::RangeIndex::AO].Init(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)lighting_pipeline::RangeIndex::AO, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

    ranges[(UINT)lighting_pipeline::RangeIndex::EMISSION].Init(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)lighting_pipeline::RangeIndex::EMISSION, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

    ranges[(UINT)lighting_pipeline::RangeIndex::MASK_MATERIAL].Init(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)lighting_pipeline::RangeIndex::MASK_MATERIAL, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

    ranges[(UINT)lighting_pipeline::RangeIndex::MASK_SHADOW].Init(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)lighting_pipeline::RangeIndex::MASK_SHADOW, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

    ranges[(UINT)lighting_pipeline::RangeIndex::DEPTH].Init(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)lighting_pipeline::RangeIndex::DEPTH, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

    // Set descriptor tables for textures
    root_paras[(UINT)lighting_pipeline::RootParameterIndex::LIGHTS].InitAsDescriptorTable(
        1, &ranges[(UINT)lighting_pipeline::RangeIndex::LIGHTS], D3D12_SHADER_VISIBILITY_PIXEL);

    root_paras[(UINT)lighting_pipeline::RootParameterIndex::ALBEDO].InitAsDescriptorTable(
        1, &ranges[(UINT)lighting_pipeline::RangeIndex::ALBEDO], D3D12_SHADER_VISIBILITY_PIXEL);

    root_paras[(UINT)lighting_pipeline::RootParameterIndex::NORMAL].InitAsDescriptorTable(
        1, &ranges[(UINT)lighting_pipeline::RangeIndex::NORMAL], D3D12_SHADER_VISIBILITY_PIXEL);

    root_paras[(UINT)lighting_pipeline::RootParameterIndex::METALNESS].InitAsDescriptorTable(
        1, &ranges[(UINT)lighting_pipeline::RangeIndex::METALNESS], D3D12_SHADER_VISIBILITY_PIXEL);

    root_paras[(UINT)lighting_pipeline::RootParameterIndex::ROUGHNESS].InitAsDescriptorTable(
        1, &ranges[(UINT)lighting_pipeline::RangeIndex::ROUGHNESS], D3D12_SHADER_VISIBILITY_PIXEL);

    root_paras[(UINT)lighting_pipeline::RootParameterIndex::SPECULAR].InitAsDescriptorTable(
        1, &ranges[(UINT)lighting_pipeline::RangeIndex::SPECULAR], D3D12_SHADER_VISIBILITY_PIXEL);

    root_paras[(UINT)lighting_pipeline::RootParameterIndex::AO].InitAsDescriptorTable(
        1, &ranges[(UINT)lighting_pipeline::RangeIndex::AO], D3D12_SHADER_VISIBILITY_PIXEL);

    root_paras[(UINT)lighting_pipeline::RootParameterIndex::EMISSION].InitAsDescriptorTable(
        1, &ranges[(UINT)lighting_pipeline::RangeIndex::EMISSION], D3D12_SHADER_VISIBILITY_PIXEL);

    root_paras[(UINT)lighting_pipeline::RootParameterIndex::MASK_MATERIAL].InitAsDescriptorTable(
        1, &ranges[(UINT)lighting_pipeline::RangeIndex::MASK_MATERIAL], D3D12_SHADER_VISIBILITY_PIXEL);

    root_paras[(UINT)lighting_pipeline::RootParameterIndex::MASK_SHADOW].InitAsDescriptorTable(
        1, &ranges[(UINT)lighting_pipeline::RangeIndex::MASK_SHADOW], D3D12_SHADER_VISIBILITY_PIXEL);

    root_paras[(UINT)lighting_pipeline::RootParameterIndex::DEPTH].InitAsDescriptorTable(
        1, &ranges[(UINT)lighting_pipeline::RangeIndex::DEPTH], D3D12_SHADER_VISIBILITY_PIXEL);

    // Initialize the root signature description.
    root_signature_desc.Init_1_1(
        (UINT)lighting_pipeline::RootParameterIndex::COUNT, root_paras, SAMPLER_COUNT, &sampler,
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
    result = dx12_util::LoadCSO("../resources/render_graph/data/lighting_pass_ps.cso", pixel_shader_binary);

    // Pipeline state setting
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};

    // Input assembler
    pso_desc.InputLayout = { lighting_pass::INPUT_LAYOUT, _countof(lighting_pass::INPUT_LAYOUT) };

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

    // Depth stencil state
    pso_desc.DepthStencilState.DepthEnable = FALSE; // Disable depth for composition pass
    pso_desc.DepthStencilState.StencilEnable = FALSE; // Disable stencil for composition pass
    pso_desc.DSVFormat = DXGI_FORMAT_UNKNOWN; // No depth-stencil format needed

    // Sampler
    pso_desc.SampleMask = UINT_MAX;
    pso_desc.SampleDesc.Count = 1;

    // Render Target Format
    pso_desc.NumRenderTargets = (uint32_t)lighting_pass::RenderTargetIndex::COUNT;
    for (uint32_t i = 0; i < (uint32_t)lighting_pass::RenderTargetIndex::COUNT; ++i)
        pso_desc.RTVFormats[i] = lighting_pass::RENDER_TARGET_FORMATS[i];

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

void LightingPipeline::SetRootParameters(
    ID3D12GraphicsCommandList* command_list, const PassAPI& pass_api)
{
    // Cast to LightingPassAPI
    const LightingPassAPI* lighting_pass_api = dynamic_cast<const LightingPassAPI*>(&pass_api);
    assert(lighting_pass_api != nullptr && "Invalid PassAPI type for LightingPipeline");

    // Set inverse view-projection matrix buffer
    SetBuffer(
        lighting_pass_api->GetInvViewProjMatrixBufferHandle(), lighting_pass_api->GetReadAccessToken(),
        command_list, (UINT)lighting_pipeline::RootParameterIndex::INV_VIEW_PROJ_MATRIX);

    // Set light config buffer
    SetBuffer(
        lighting_pass_api->GetLightConfigBufferHandle(), lighting_pass_api->GetReadAccessToken(),
        command_list, (UINT)lighting_pipeline::RootParameterIndex::LIGHT_CONFIG_BUFFER);

    // Set lights structured buffer
    SetStructuredBuffer(
        lighting_pass_api->GetLightsBufferHandle(), lighting_pass_api->GetReadAccessToken(),
        command_list, (UINT)lighting_pipeline::RootParameterIndex::LIGHTS);

    // Set albedo texture
    SetTexture2D(
        lighting_pass_api->GetAlbedoTextureHandle(), lighting_pass_api->GetReadAccessToken(),
        command_list, (UINT)lighting_pipeline::RootParameterIndex::ALBEDO);

    // Set normal texture
    SetTexture2D(
        lighting_pass_api->GetNormalTextureHandle(), lighting_pass_api->GetReadAccessToken(),
        command_list, (UINT)lighting_pipeline::RootParameterIndex::NORMAL);

    // Set metalness texture
    SetTexture2D(
        lighting_pass_api->GetMetalnessTextureHandle(), lighting_pass_api->GetReadAccessToken(),
        command_list, (UINT)lighting_pipeline::RootParameterIndex::METALNESS);

    // Set roughness texture
    SetTexture2D(
        lighting_pass_api->GetRoughnessTextureHandle(), lighting_pass_api->GetReadAccessToken(),
        command_list, (UINT)lighting_pipeline::RootParameterIndex::ROUGHNESS);

    // Set specular texture
    SetTexture2D(
        lighting_pass_api->GetSpecularTextureHandle(), lighting_pass_api->GetReadAccessToken(),
        command_list, (UINT)lighting_pipeline::RootParameterIndex::SPECULAR);

    // Set ambient occlusion texture
    SetTexture2D(
        lighting_pass_api->GetAOTextureHandle(), lighting_pass_api->GetReadAccessToken(),
        command_list, (UINT)lighting_pipeline::RootParameterIndex::AO);

    // Set emission texture
    SetTexture2D(
        lighting_pass_api->GetEmissionTextureHandle(), lighting_pass_api->GetReadAccessToken(),
        command_list, (UINT)lighting_pipeline::RootParameterIndex::EMISSION);

    // Set mask material texture
    SetTexture2D(
        lighting_pass_api->GetMaskMaterialTextureHandle(), lighting_pass_api->GetReadAccessToken(),
        command_list, (UINT)lighting_pipeline::RootParameterIndex::MASK_MATERIAL);

    // Set mask shadow texture
    SetTexture2D(
        lighting_pass_api->GetMaskShadowTextureHandle(), lighting_pass_api->GetReadAccessToken(),
        command_list, (UINT)lighting_pipeline::RootParameterIndex::MASK_SHADOW);

    // Set depth texture
    SetTexture2D(
        lighting_pass_api->GetDepthStencilTextureHandle(), lighting_pass_api->GetReadAccessToken(),
        command_list, (UINT)lighting_pipeline::RootParameterIndex::DEPTH);
}

} // namespace render_graph