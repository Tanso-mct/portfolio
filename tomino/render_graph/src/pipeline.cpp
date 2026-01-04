#include "render_graph/src/pch.h"
#include "render_graph/include/pipeline.h"

using Microsoft::WRL::ComPtr;

namespace render_graph
{

void Pipeline::SetPipeline(ID3D12GraphicsCommandList* command_list)
{
    assert(command_list != nullptr && "Command list is null.");
    assert(rootSignature_ != nullptr && "Root signature is not set.");
    assert(pipelineState_ != nullptr && "Pipeline state is not set.");

    // Set root signature
    command_list->SetGraphicsRootSignature(rootSignature_.Get());

    // Set pipeline state
    command_list->SetPipelineState(pipelineState_.Get());
}

// Helper function to set a buffer as a root parameter
RENDER_GRAPH_DLL void SetBuffer(
    const ResourceHandle* handle, const ResourceAccessToken& token, 
    ID3D12GraphicsCommandList* command_list, UINT root_parameter_index)
{
    // Get buffer resource
    const dx12_util::Resource& buffer_resource
        = ResourceManager::GetInstance().GetReadResource(handle, token);
    const dx12_util::Buffer* buffer
        = dynamic_cast<const dx12_util::Buffer*>(&buffer_resource);
    assert(buffer != nullptr && "Failed to cast to Buffer for buffer resource.");

    // Set buffer
    command_list->SetGraphicsRootConstantBufferView(root_parameter_index, buffer->GetGPUVirtualAddress());
}

// Helper function to set a structured buffer as a root parameter
RENDER_GRAPH_DLL void SetStructuredBuffer(
    const ResourceHandle* handle, const ResourceAccessToken& token, 
    ID3D12GraphicsCommandList* command_list, UINT root_parameter_index)
{
    // Get structured buffer resource
    const dx12_util::Resource& structured_buffer_resource
        = ResourceManager::GetInstance().GetReadResource(handle, token);
    const dx12_util::StructuredBuffer* structured_buffer
        = dynamic_cast<const dx12_util::StructuredBuffer*>(&structured_buffer_resource);
    assert(structured_buffer != nullptr && "Failed to cast to StructuredBuffer for structured buffer resource.");

    // Set structured buffer SRV descriptor table
    command_list->SetGraphicsRootDescriptorTable(root_parameter_index, structured_buffer->GetSrvGpuHandle());
}

// Helper function to set a Texture2D as a root parameter
RENDER_GRAPH_DLL void SetTexture2D(
    const ResourceHandle* handle, const ResourceAccessToken& token, 
    ID3D12GraphicsCommandList* command_list, UINT root_parameter_index)
{
    // Get texture2D resource
    const dx12_util::Resource& texture_resource
        = ResourceManager::GetInstance().GetReadResource(handle, token);
    const dx12_util::Texture2D* texture
        = dynamic_cast<const dx12_util::Texture2D*>(&texture_resource);
    assert(texture != nullptr && "Failed to cast to Texture2D for texture resource.");

    // Set texture SRV descriptor table
    command_list->SetGraphicsRootDescriptorTable(root_parameter_index, texture->GetSrvGpuHandle());
}

} // namespace render_graph