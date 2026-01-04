#include "render_graph/src/pch.h"
#include "render_graph/include/phong_pipeline.h"

using Microsoft::WRL::ComPtr;

#include "utility_header/logger.h"
#include "directx12_util/include/wrapper.h"
#include "directx12_util/include/d3dx12.h"
#include "directx12_util/include/helper.h"

#include "render_graph/include/geometry_pass.h"
#include "render_graph/include/material_manager.h"
#include "render_graph/include/phong_material.h"

namespace render_graph
{

bool PhongPipeline::Setup()
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
    CD3DX12_ROOT_PARAMETER1 root_params[(UINT)phong_pipeline::RootParameterIndex::COUNT];

    // Set constant buffers
    root_params[(UINT)phong_pipeline::RootParameterIndex::VIEW_PROJ_MATRIX].InitAsConstantBufferView(
        (UINT)phong_pipeline::RootParameterIndex::VIEW_PROJ_MATRIX, 0, 
        D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE, D3D12_SHADER_VISIBILITY_VERTEX);

    root_params[(UINT)phong_pipeline::RootParameterIndex::WORLD_MATRIX].InitAsConstantBufferView(
        (UINT)phong_pipeline::RootParameterIndex::WORLD_MATRIX, 0, 
        D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE, D3D12_SHADER_VISIBILITY_VERTEX);

    root_params[(UINT)phong_pipeline::RootParameterIndex::MATERIAL].InitAsConstantBufferView(
        (UINT)phong_pipeline::RootParameterIndex::MATERIAL, 0,
        D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE, D3D12_SHADER_VISIBILITY_PIXEL);

    // Creating RANGE for textures
    CD3DX12_DESCRIPTOR_RANGE1 ranges[(UINT)phong_pipeline::RangeIndex::COUNT];

    // Set SRVs for textures
    ranges[(UINT)phong_pipeline::RangeIndex::ALBEDO].Init(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)phong_pipeline::RangeIndex::ALBEDO, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

    ranges[(UINT)phong_pipeline::RangeIndex::NORMAL].Init(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)phong_pipeline::RangeIndex::NORMAL, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

    ranges[(UINT)phong_pipeline::RangeIndex::AO].Init(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)phong_pipeline::RangeIndex::AO, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

    ranges[(UINT)phong_pipeline::RangeIndex::SPECULAR].Init(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)phong_pipeline::RangeIndex::SPECULAR, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

    ranges[(UINT)phong_pipeline::RangeIndex::ROUGHNESS].Init(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)phong_pipeline::RangeIndex::ROUGHNESS, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

    ranges[(UINT)phong_pipeline::RangeIndex::METALNESS].Init(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)phong_pipeline::RangeIndex::METALNESS, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

    ranges[(UINT)phong_pipeline::RangeIndex::EMISSION].Init(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)phong_pipeline::RangeIndex::EMISSION, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

    // Set descriptor tables for textures
    root_params[(UINT)phong_pipeline::RootParameterIndex::ALBEDO_TEXTURE].InitAsDescriptorTable(
        1, &ranges[(UINT)phong_pipeline::RangeIndex::ALBEDO], D3D12_SHADER_VISIBILITY_PIXEL);

    root_params[(UINT)phong_pipeline::RootParameterIndex::NORMAL_TEXTURE].InitAsDescriptorTable(
        1, &ranges[(UINT)phong_pipeline::RangeIndex::NORMAL], D3D12_SHADER_VISIBILITY_PIXEL);

    root_params[(UINT)phong_pipeline::RootParameterIndex::AO_TEXTURE].InitAsDescriptorTable(
        1, &ranges[(UINT)phong_pipeline::RangeIndex::AO], D3D12_SHADER_VISIBILITY_PIXEL);

    root_params[(UINT)phong_pipeline::RootParameterIndex::SPECULAR_TEXTURE].InitAsDescriptorTable(
        1, &ranges[(UINT)phong_pipeline::RangeIndex::SPECULAR], D3D12_SHADER_VISIBILITY_PIXEL);

    root_params[(UINT)phong_pipeline::RootParameterIndex::ROUGHNESS_TEXTURE].InitAsDescriptorTable(
        1, &ranges[(UINT)phong_pipeline::RangeIndex::ROUGHNESS], D3D12_SHADER_VISIBILITY_PIXEL);

    root_params[(UINT)phong_pipeline::RootParameterIndex::METALNESS_TEXTURE].InitAsDescriptorTable(
        1, &ranges[(UINT)phong_pipeline::RangeIndex::METALNESS], D3D12_SHADER_VISIBILITY_PIXEL);

    root_params[(UINT)phong_pipeline::RootParameterIndex::EMISSION_TEXTURE].InitAsDescriptorTable(
        1, &ranges[(UINT)phong_pipeline::RangeIndex::EMISSION], D3D12_SHADER_VISIBILITY_PIXEL);

    // Initialize the root signature description.
    root_signature_desc.Init_1_1(
        (UINT)phong_pipeline::RootParameterIndex::COUNT, root_params, SAMPLER_COUNT, &sampler,
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
    result = dx12_util::LoadCSO("../resources/render_graph/data/geometry_pass_vs.cso", vertex_shader_binary);

    // Load pixel shader binary
    dx12_util::ShaderBinary pixel_shader_binary;
    result = dx12_util::LoadCSO("../resources/render_graph/data/geometry_pass_phong_ps.cso", pixel_shader_binary);

    // Pipeline state setting
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};

    // Input assembler
    pso_desc.InputLayout = { geometry_pass::INPUT_LAYOUT, _countof(geometry_pass::INPUT_LAYOUT) };

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
    pso_desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    pso_desc.DSVFormat = geometry_pass::DEPTH_STENCIL_FORMAT;

    // Sampler
    pso_desc.SampleMask = UINT_MAX;
    pso_desc.SampleDesc.Count = 1;

    // Render Target Format
    pso_desc.NumRenderTargets = (UINT)geometry_pass::GBufferIndex::COUNT;
    for (UINT i = 0; i < (UINT)geometry_pass::GBufferIndex::COUNT; ++i)
        pso_desc.RTVFormats[i] = geometry_pass::GBUFFER_FORMATS[i];

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

    return true; // Success
}

void PhongPipeline::SetRootParameters(
    ID3D12GraphicsCommandList* command_list, const PassAPI& pass_api)
{
    // Cast to GeometryPassAPI
    const GeometryPassAPI* geometry_pass_api = dynamic_cast<const GeometryPassAPI*>(&pass_api);
    assert(geometry_pass_api && "Failed to cast PassAPI to GeometryPassAPI");

    // Set view-projection matrix buffer
    SetBuffer(
        geometry_pass_api->GetViewProjMatrixBufferHandle(), geometry_pass_api->GetCurrentReadToken(),
        command_list, (UINT)phong_pipeline::RootParameterIndex::VIEW_PROJ_MATRIX);

    // Set world matrix buffer
    SetBuffer(
        geometry_pass_api->GetDrawingWorldMatrixBufferHandle(), geometry_pass_api->GetCurrentReadToken(),
        command_list, (UINT)phong_pipeline::RootParameterIndex::WORLD_MATRIX);

    // Get phong material
    const Material& material = MaterialManager::GetInstance().GetMaterial(geometry_pass_api->GetDrawingMaterialHandle());
    const PhongMaterial* phong_material = dynamic_cast<const PhongMaterial*>(&material);
    assert(phong_material && "Provided material is not PhongMaterial.");

    // Set material buffer
    SetBuffer(
        phong_material->GetBufferHandle(), geometry_pass_api->GetCurrentReadToken(),
        command_list, (UINT)phong_pipeline::RootParameterIndex::MATERIAL);

    // Set albedo texture
    SetTexture2D(
        phong_material->GetAlbedoTextureHandle(), geometry_pass_api->GetCurrentReadToken(),
        command_list, (UINT)phong_pipeline::RootParameterIndex::ALBEDO_TEXTURE);

    // Set normal texture
    SetTexture2D(
        phong_material->GetNormalTextureHandle(), geometry_pass_api->GetCurrentReadToken(),
        command_list, (UINT)phong_pipeline::RootParameterIndex::NORMAL_TEXTURE);

    // Set AO texture
    SetTexture2D(
        phong_material->GetAOTextureHandle(), geometry_pass_api->GetCurrentReadToken(),
        command_list, (UINT)phong_pipeline::RootParameterIndex::AO_TEXTURE);

    // Set specular texture
    SetTexture2D(
        phong_material->GetSpecularTextureHandle(), geometry_pass_api->GetCurrentReadToken(),
        command_list, (UINT)phong_pipeline::RootParameterIndex::SPECULAR_TEXTURE);

    // Set roughness texture
    SetTexture2D(
        phong_material->GetRoughnessTextureHandle(), geometry_pass_api->GetCurrentReadToken(),
        command_list, (UINT)phong_pipeline::RootParameterIndex::ROUGHNESS_TEXTURE);

    // Set metalness texture
    SetTexture2D(
        phong_material->GetMetalnessTextureHandle(), geometry_pass_api->GetCurrentReadToken(),
        command_list, (UINT)phong_pipeline::RootParameterIndex::METALNESS_TEXTURE);

    // Set emission texture
    SetTexture2D(
        phong_material->GetEmissionTextureHandle(), geometry_pass_api->GetCurrentReadToken(),
        command_list, (UINT)phong_pipeline::RootParameterIndex::EMISSION_TEXTURE);
}

} // namespace render_graph