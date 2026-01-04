#pragma once

#include <wrl/client.h>
#include <d3d12.h>

#include "render_graph/include/dll_config.h"
#include "render_graph/include/resource_handle.h"
#include "render_graph/include/resource_manager.h"

namespace render_graph
{

// Interface for passes api
class PassAPI
{
public:
    PassAPI() = default;
    virtual ~PassAPI() = default;

    // Get the pass API
    virtual PassAPI& GetPassAPI() = 0;

    // Get the pass API (const)
    virtual const PassAPI& GetPassAPI() const = 0;
};

// Base class for pipelines
class RENDER_GRAPH_DLL Pipeline
{
public:
    Pipeline() = default;
    virtual ~Pipeline() = default;

    // Setup the pipeline
    virtual bool Setup() = 0;

    // Set the pipeline to the command list
    void SetPipeline(ID3D12GraphicsCommandList* command_list);

    // Set root parameters specific to the derived pipeline
    virtual void SetRootParameters(
        ID3D12GraphicsCommandList* command_list, const PassAPI& pass_api) = 0;

protected:
    // Root signature for the geometry pass
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

    // Pipeline state object for the geometry pass
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;

};

// Helper function to set a buffer as a root parameter
RENDER_GRAPH_DLL void SetBuffer(
    const ResourceHandle* handle, const ResourceAccessToken& token, 
    ID3D12GraphicsCommandList* command_list, UINT root_parameter_index);

// Helper function to set a structured buffer as a root parameter
RENDER_GRAPH_DLL void SetStructuredBuffer(
    const ResourceHandle* handle, const ResourceAccessToken& token, 
    ID3D12GraphicsCommandList* command_list, UINT root_parameter_index);

// Helper function to set a Texture2D as a root parameter
RENDER_GRAPH_DLL void SetTexture2D(
    const ResourceHandle* handle, const ResourceAccessToken& token, 
    ID3D12GraphicsCommandList* command_list, UINT root_parameter_index);

} // namespace render_graph