#include "example/src/pch.h"
#include "example/include/window_game_example/facade_factory.h"

#include "example/include/window_game_example/monitor_keyboard.h"
#include "example/include/window_game_example/monitor_mouse.h"

#include "wbp_d3d12/plugin.h"
#pragma comment(lib, "wbp_d3d12.lib")

#include "wbp_render/plugin.h"
#pragma comment(lib, "wbp_render.lib")

std::unique_ptr<wb::IWindowFacade> example::GameExampleWindowFacadeFactory::Create() const
{
    std::unique_ptr<wb::IWindowFacade> facade = std::make_unique<wbp_d3d12::WindowD3D12Facade>
    (
        wbp_render::RENDER_TARGET_COUNT, 
        wbp_render::DEPTH_STENCIL_COUNT
    );

    {
        std::unique_ptr<wb::IWindowContext> context = std::make_unique<wb::WindowContext>();
        context->Name() = L"GameExample Window";

        facade->SetContext(std::move(context));
    }

    {
        wbp_d3d12::IWindowD3D12Facade *d3d12WindowFacade = wb::As<wbp_d3d12::IWindowD3D12Facade>(facade.get());
        
        std::unique_ptr<wbp_d3d12::ISwapChainContext> swapChainContext = std::make_unique<wbp_d3d12::SwapChainContext>();
        d3d12WindowFacade->SetSwapChainContext(std::move(swapChainContext));

        std::unique_ptr<wbp_d3d12::IRenderTargetContext> renderTargetContext = std::make_unique<wbp_d3d12::RenderTargetContext>();
        d3d12WindowFacade->SetRenderTargetContext(std::move(renderTargetContext));

        std::unique_ptr<wbp_d3d12::IFenceContext> fenceContext = std::make_unique<wbp_d3d12::FenceContext>();
        d3d12WindowFacade->SetFenceContext(std::move(fenceContext));
    }

    facade->AddMonitorID(example::GameExampleKeyboardMonitorID());
    facade->AddMonitorID(example::GameExampleMouseMonitorID());

    return facade;
}