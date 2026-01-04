#include "render_graph/src/pch.h"
#include "render_graph/include/composition_pipeline.h"

using Microsoft::WRL::ComPtr;

#include "utility_header/logger.h"
#include "directx12_util/include/wrapper.h"
#include "directx12_util/include/d3dx12.h"
#include "directx12_util/include/helper.h"

#include "render_graph/include/composition_pass.h"

namespace render_graph
{

bool CompositionPipeline::Setup()
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
    CD3DX12_ROOT_PARAMETER1 rootParameters[(UINT)composition_pipeline::RootParameterIndex::COUNT];

    // Creating RANGE for textures
    CD3DX12_DESCRIPTOR_RANGE1 ranges[(UINT)composition_pipeline::RangeIndex::COUNT];

    // Set SRVs for textures
    ranges[(UINT)composition_pipeline::RangeIndex::POST_PROCESS_TEXTURE].Init(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

    ranges[(UINT)composition_pipeline::RangeIndex::UI_TEXTURE].Init(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

    // Set texture SRV to root parameter
    rootParameters[(UINT)composition_pipeline::RootParameterIndex::POST_PROCESS_TEXTURE].InitAsDescriptorTable(
        1, &ranges[(UINT)composition_pipeline::RangeIndex::POST_PROCESS_TEXTURE], D3D12_SHADER_VISIBILITY_PIXEL);

    rootParameters[(UINT)composition_pipeline::RootParameterIndex::UI_TEXTURE].InitAsDescriptorTable(
        1, &ranges[(UINT)composition_pipeline::RangeIndex::UI_TEXTURE], D3D12_SHADER_VISIBILITY_PIXEL);

    // Initialize the root signature description.
    root_signature_desc.Init_1_1(
        (UINT)composition_pipeline::RootParameterIndex::COUNT, rootParameters, SAMPLER_COUNT, &sampler,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    // Integrate parameters to create configuration binaries
    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    hr = D3DX12SerializeVersionedRootSignature(
        &root_signature_desc, feature_data.HighestVersion, signature.GetAddressOf(), error.GetAddressOf());
    if (FAILED(hr))
    {
        utility_header::ConsoleLogErr(
            {"Failed to serialize root signature for GeometryPass."}, hr,
            __FILE__, __LINE__, __FUNCTION__);
        return false; // Setup failed
    }

    // Create ID3D12RootSignature with the binary created by merging.
    hr = device.Get()->CreateRootSignature(
        0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(rootSignature_.GetAddressOf()));
    if (FAILED(hr))
    {
        utility_header::ConsoleLogErr(
            {"Failed to create root signature for GeometryPass."}, hr,
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
    result = dx12_util::LoadCSO("../resources/render_graph/data/composition_pass_ps.cso", pixel_shader_binary);

    // Create input layout
    D3D12_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };

    // Pipeline state setting
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};

    // Input assembler
    pso_desc.InputLayout = { layout, _countof(layout) };

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
    pso_desc.NumRenderTargets = 1;
    pso_desc.RTVFormats[0] = composition_pass::SWAP_CHAIN_FORMAT;

    // Primitive topology
    pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    // Create pipeline state object
    hr = device.Get()->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&pipelineState_));
    if (FAILED(hr))
    {
        utility_header::ConsoleLogErr(
            {"Failed to create pipeline state object for GeometryPass."}, hr,
            __FILE__, __LINE__, __FUNCTION__);
        return false; // Setup failed
    }
    
    return true; // Setup successful
}

void CompositionPipeline::SetRootParameters(
    ID3D12GraphicsCommandList* command_list, const PassAPI& pass_api)
{
    // Cast PassAPI to CompositionPassAPI
    const CompositionPassAPI& composition_pass_api = 
        dynamic_cast<const CompositionPassAPI&>(pass_api);

    // Get post-process texture
    const dx12_util::Resource& post_process_texture_resource
        = ResourceManager::GetInstance().GetReadResource(
            composition_pass_api.GetPostProcessTextureHandle(), composition_pass_api.GetReadToken());
    const dx12_util::Texture2D* post_process_texture
        = dynamic_cast<const dx12_util::Texture2D*>(&post_process_texture_resource);
    assert(post_process_texture != nullptr && "Failed to cast Resource to Texture2D for post-process texture.");

    // Set post-process texture SRV
    command_list->SetGraphicsRootDescriptorTable(
        (UINT)composition_pipeline::RootParameterIndex::POST_PROCESS_TEXTURE,
        post_process_texture->GetSrvGpuHandle());

    // Get UI texture
    const dx12_util::Resource& ui_texture_resource
        = ResourceManager::GetInstance().GetReadResource(
            composition_pass_api.GetUITextureHandle(), composition_pass_api.GetReadToken());
    const dx12_util::Texture2D* ui_texture
        = dynamic_cast<const dx12_util::Texture2D*>(&ui_texture_resource);
    assert(ui_texture != nullptr && "Failed to cast Resource to Texture2D for UI texture.");

    // Set UI texture SRV
    command_list->SetGraphicsRootDescriptorTable(
        (UINT)composition_pipeline::RootParameterIndex::UI_TEXTURE,
        ui_texture->GetSrvGpuHandle());
}

} // namespace render_graph