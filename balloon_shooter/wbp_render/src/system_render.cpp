#include "wbp_render/src/pch.h"
#include "wbp_render/include/system_render.h"

#include "wbp_render/include/component_camera.h"
#include "wbp_render/include/component_mesh_renderer.h"
#include "wbp_render/include/component_sprite_renderer.h"

#include "wbp_render/include/render_pass_model_forward.h"
#include "wbp_render/include/render_pass_sprite_forward.h"

#include <DirectXMath.h>
using namespace DirectX;

using Microsoft::WRL::ComPtr;

#include "wbp_d3d12/plugin.h"
#pragma comment(lib, "wbp_d3d12.lib")

#include "wbp_identity/plugin.h"
#pragma comment(lib, "wbp_identity.lib")

#include "wbp_transform/plugin.h"
#pragma comment(lib, "wbp_transform.lib")

const WBP_RENDER_API size_t &wbp_render::RenderSystemID()
{
    static size_t id = wb::IDFactory::CreateSystemID();
    return id;
}

wbp_render::RenderSystem::~RenderSystem()
{
    // Get gpu context
    wbp_d3d12::GPUContext &gpuContext = wbp_d3d12::GPUContext::GetInstance();

    // Fence作成
    ComPtr<ID3D12Fence> fence;
    gpuContext.GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    UINT64 fenceValue = 1;

    // コマンドキューにSignal
    gpuContext.GetCommandQueue()->Signal(fence.Get(), fenceValue);

    // Fence値に到達するまで待つ
    if (fence->GetCompletedValue() < fenceValue) 
    {
        HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        fence->SetEventOnCompletion(fenceValue, fenceEvent);
        WaitForSingleObject(fenceEvent, INFINITE);
        CloseHandle(fenceEvent);
    }
}

const size_t &wbp_render::RenderSystem::GetID() const
{
    return RenderSystemID();
}

void wbp_render::RenderSystem::Initialize(wb::IAssetContainer &assetContainer)
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

    wbp_d3d12::CreateBuffer
    (
        gpuContext.GetDevice(),
        cameraViewMatBuff_, sizeof(XMMATRIX)
    );

    wbp_d3d12::CreateBuffer
    (
        gpuContext.GetDevice(),
        cameraProjectionMatBuff_, sizeof(XMMATRIX)
    );

    // Set render passes. The order of addition to vector is the order of execution.
    passes_.clear();
    passes_.emplace_back(std::make_unique<ModelForwardRenderPass>());
    passes_.emplace_back(std::make_unique<SpriteForwardRenderPass>());
}

void wbp_render::RenderSystem::Update(const wb::SystemArgument &args)
{
    // Get containers to use
    wb::IWindowContainer &windowContainer = args.containerStorage_.GetContainer<wb::IWindowContainer>();

    // Get the window facade for the current window
    wb::IWindowFacade &window = windowContainer.Get(args.belongWindowID_);
    wbp_d3d12::IWindowD3D12Facade *d3d12Window = wb::As<wbp_d3d12::IWindowD3D12Facade>(&window);
    if (d3d12Window == nullptr) return; // Skip if not a D3D12 window

    if (window.NeedsResize())
    {
        d3d12Window->WaitForGPU();
        window.Resized();
    }

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

    if (!passesInitialized_)
    {
        for (const std::unique_ptr<IRenderPass> &pass : passes_)
        {
            pass->Initialize(d3d12Window->GetCommandAllocator());
            d3d12Window->WaitForGPU();
        }
        passesInitialized_ = true;
    }

    // Get camera
    std::unique_ptr<wb::IOptionalValue> cameraEntityID = nullptr;
    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(wbp_render::CameraComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue; // Skip if entity is null

        wb::IComponent *cameraComponent = entity->GetComponent(wbp_render::CameraComponentID(), args.componentContainer_);
        wbp_render::ICameraComponent *camera = wb::As<wbp_render::ICameraComponent>(cameraComponent);
        if (camera == nullptr) continue; // Skip if not a camera component

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), args.componentContainer_);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        if (identity == nullptr)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {
                    "Camera entity does not have IdentityComponent.",
                    "CameraComponent requires IdentityComponent to be set.",
                }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_RENDER", err);
            wb::ThrowRuntimeError(err);
        }

        if (!identity->IsActiveSelf())
        {
            // Skip if the camera entity is not active
            continue;
        }

        if (cameraEntityID == nullptr)
        {
            cameraEntityID = id->Clone();
        }
        else
        {
            // If multiple cameras are found, log a warning and use the first one
            std::string msg = wb::CreateMessage
            ({
                "Multiple cameras found. Using the first one.",
                "Entity ID: " + std::to_string((*cameraEntityID)())
            });
            wb::ConsoleLogWrn(msg);
            break;
        }
    }

    // Get the camera entity
    if (cameraEntityID == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "Camera entity not found."
            }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WBP_RENDER", err);
        wb::ThrowRuntimeError(err);
    }
    wb::IEntity *cameraEntity = args.entityContainer_.PtrGet(*cameraEntityID);

    XMMATRIX cameraViewMat = XMMatrixIdentity();
    {
        wb::IComponent *transformComponent = cameraEntity->GetComponent(wbp_transform::TransformComponentID(), args.componentContainer_);
        wbp_transform::ITransformComponent *transform = wb::As<wbp_transform::ITransformComponent>(transformComponent);
        if (transform == nullptr)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {
                    "Camera entity does not have TransformComponent.",
                    "CameraComponent requires TransformComponent to be set.",
                }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_RENDER", err);
            wb::ThrowRuntimeError(err);
        }

        cameraViewMat = XMMatrixLookAtLH
        (
            XMLoadFloat3(&transform->GetPosition()),
            transform->GetForward() + XMLoadFloat3(&transform->GetPosition()),
            transform->GetUp()
        );

    }

    {
        XMMATRIX transposedMat = XMMatrixTranspose(cameraViewMat);
        wbp_d3d12::UpdateBuffer
        (
            cameraViewMatBuff_,
            &transposedMat, sizeof(XMMATRIX)
        );
    }

    XMMATRIX cameraProjectionMat = XMMatrixIdentity();
    {
        wb::IComponent *cameraComponent = cameraEntity->GetComponent(wbp_render::CameraComponentID(), args.componentContainer_);
        wbp_render::ICameraComponent *camera = wb::As<wbp_render::ICameraComponent>(cameraComponent);

        // Get aspect ratio
        float aspectRatio = static_cast<float>(window.GetClientWidth()) / window.GetClientHeight();

        cameraProjectionMat = XMMatrixPerspectiveFovLH
        (
            XMConvertToRadians(camera->GetFieldOfView()),
            aspectRatio,
            camera->GetNearZ(),
            camera->GetFarZ()
        );
    }

    {
        XMMATRIX transposedMat = XMMatrixTranspose(cameraProjectionMat);
        wbp_d3d12::UpdateBuffer
        (
            cameraProjectionMatBuff_,
            &transposedMat, sizeof(XMMATRIX)
        );
    }

    d3d12Window->ResetCommand(nullptr);
    d3d12Window->SetBarrierToRenderTarget();
    d3d12Window->SetRenderTarget(DEPTH_STENCIL_FOR_DRAW);

    d3d12Window->ClearViews(CLEAR_COLOR, DEPTH_STENCIL_FOR_DRAW);

    d3d12Window->SetBarrierToPresent();
    d3d12Window->CloseCommand();

    // Prepare command lists for execution
    std::vector<ID3D12CommandList*> commandLists;
    commandLists.emplace_back(d3d12Window->GetCommandList());

    // Execute render passes
    for (const std::unique_ptr<wbp_render::IRenderPass> &renderPass : passes_)
    {
        ID3D12GraphicsCommandList *commandList = renderPass->Execute
        (
            d3d12Window->GetCurrentFrameIndex(),
            cameraViewMatBuff_.Get(), cameraProjectionMatBuff_.Get(),
            args
        );

        if (commandList != nullptr)
        {
            commandLists.emplace_back(commandList);
        }
    }

    d3d12Window->WaitForGPU();
    wbp_d3d12::ExecuteCommand(gpuContext.GetCommandQueue(), commandLists.size(), commandLists);

    d3d12Window->Present();
    d3d12Window->WaitThisFrameForGPU();
    
}

namespace wbp_render
{
    WB_REGISTER_SYSTEM(RenderSystem, RenderSystemID());

} // namespace wbp_render