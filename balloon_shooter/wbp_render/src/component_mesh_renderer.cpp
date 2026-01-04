#include "wbp_render/src/pch.h"
#include "wbp_render/include/component_mesh_renderer.h"

#include <DirectXMath.h>
using namespace DirectX;

#include "wbp_d3d12/plugin.h"
#pragma comment(lib, "wbp_d3d12.lib")

const WBP_RENDER_API size_t &wbp_render::MeshRendererComponentID()
{
    static size_t id = wb::IDFactory::CreateComponentID();
    return id;
}

const size_t &wbp_render::MeshRenderComponent::GetID() const
{
    return MeshRendererComponentID();
}

wbp_render::MeshRenderComponent::MeshRenderComponent()
{
    // Get gpu context
    wbp_d3d12::GPUContext &gpuContext = wbp_d3d12::GPUContext::GetInstance();
    if (!gpuContext.IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "GPU context is not created."
            }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WBP_RENDER", err);
        wb::ThrowRuntimeError(err);
    }

    // Create world matrix buffer
    wbp_d3d12::CreateBuffer(gpuContext.GetDevice(), worldMatBuffer_, sizeof(XMMATRIX));

    // Create color configuration buffer
    wbp_d3d12::CreateBuffer
    (
        gpuContext.GetDevice(),
        colorConfigBuffer_, sizeof(wbp_render::ModelColorConfig)
    );

    // Create texture asset IDs vector with size equal to TextureType::Count
    textureAssetIDs_.resize(static_cast<size_t>(TextureType::Count));
    textureAssetIDsSet_.resize(static_cast<size_t>(TextureType::Count), false);
}

bool wbp_render::MeshRenderComponent::GetTextureAssetID(TextureType textureType, size_t &idDist) const
{
    if (textureType >= TextureType::Count)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Invalid texture type." }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WBP_RENDER", err);
        wb::ThrowRuntimeError(err);
    }

    if (textureAssetIDsSet_[static_cast<size_t>(textureType)])
    {
        idDist = textureAssetIDs_[static_cast<size_t>(textureType)];
        return true;
    }
    else
    {
        idDist = 0; // Return 0 if the texture asset ID is not set
        return false;
    }
}

void wbp_render::MeshRenderComponent::AddTextureAssetID(const size_t &textureAssetID, TextureType textureType)
{
    if (textureType >= TextureType::Count)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Invalid texture type." }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WBP_RENDER", err);
        wb::ThrowRuntimeError(err);
    }

    textureAssetIDs_[static_cast<size_t>(textureType)] = textureAssetID;
    textureAssetIDsSet_[static_cast<size_t>(textureType)] = true;
}

namespace wbp_render
{
    WB_REGISTER_COMPONENT(MeshRendererComponentID(), MeshRenderComponent);

} // namespace wbp_render