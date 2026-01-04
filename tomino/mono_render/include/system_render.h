#pragma once
#include "mono_render/include/dll_config.h"
#include "riaecs/riaecs.h"

#include "mono_service/include/service_registry.h"
#include "mono_adapter/include/service_adapter.h"
#include "mono_render/include/material_handle_manager.h"

namespace mono_render
{
    // The number of back buffers
    constexpr uint32_t BACK_BUFFER_COUNT = 2;

    class MONO_RENDER_API SystemRender : public riaecs::ISystem
    {
    private:
        // Material handle manager
        std::unique_ptr<MaterialHandleManager> material_handle_manager_ = nullptr;

        // Material handle ID generator
        std::unique_ptr<MaterialHandleIDGenerator> material_handle_id_generator_ = nullptr;

        // Service registry
        std::unique_ptr<mono_service::ServiceRegistry> service_registry_ = nullptr;

        // Graphics service adapter
        std::unique_ptr<mono_adapter::GraphicsServiceAdapter> graphics_service_adapter_ = nullptr;

    public:
        SystemRender();
        ~SystemRender() override;

        /***************************************************************************************************************
         * ISystem Implementation
        /**************************************************************************************************************/

        bool Update
        (
            riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
            riaecs::ISystemLoopCommandQueue &systemLoopCmdQueue
        ) override;
    };
    extern MONO_RENDER_API riaecs::SystemFactoryRegistrar<SystemRender> SystemRenderID;

} // namespace mono_render