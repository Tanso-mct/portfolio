#include "mono_forge_app_template/src/pch.h"
#include "mono_forge_app_template/include/systems.h"

#include "mono_asset_service/include/asset_service.h"
#include "mono_transform_service/include/transform_service.h"
#include "mono_window_service/include/window_service.h"
#include "mono_graphics_service/include/graphics_service.h"
#include "mono_entity_archive_service/include/entity_archive_service.h"

//SYSTEMS_INCLUDE_BEGIN//
#include "mono_meta_extension/include/meta_system.h"
#include "mono_asset_extension/include/asset_request_processor_system.h"
#include "mono_scene_extension/include/scene_system.h"
#include "mono_graphics_extension/include/graphics_system.h"
//SYSTEMS_INCLUDE_END//

namespace mono_forge_app_template
{

ecs::SystemIDGenerator g_system_id_generator;

std::vector<std::function<void(mono_forge_app_template::ECSHub&, mono_service::ServiceProxyManager&)>> g_system_registrations = 
{
    //SYSTEM_REGISTRATION_BEGIN//
    {
        [](mono_forge_app_template::ECSHub& ecs_hub, mono_service::ServiceProxyManager& service_proxy_manager)
        {
            ecs_hub.RegisterSystem<mono_meta_extension::MetaSystem>
            (

            );
        }
    },
    {
        [](mono_forge_app_template::ECSHub& ecs_hub, mono_service::ServiceProxyManager& service_proxy_manager)
        {
            ecs_hub.RegisterSystem<mono_asset_extension::AssetRequestProcessorSystem>
            (
                service_proxy_manager.GetServiceProxy(mono_asset_service::AssetServiceHandle::ID()).Clone()
            );
        }
    },
    {
        [](mono_forge_app_template::ECSHub& ecs_hub, mono_service::ServiceProxyManager& service_proxy_manager)
        {
            ecs_hub.RegisterSystem<mono_scene_extension::SceneSystem>
            (

            );
        }
    },
    {
        [](mono_forge_app_template::ECSHub& ecs_hub, mono_service::ServiceProxyManager& service_proxy_manager)
        {
            ecs_hub.RegisterSystem<mono_graphics_extension::GraphicsSystem>
            (
                service_proxy_manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID()).Clone()
            );
        }
    },
    //SYSTEM_REGISTRATION_END//
};

std::vector<ecs::SystemID> g_system_execution_order = 
{
    //SYSTEM_EXECUTION_ORDER_BEGIN//
    mono_meta_extension::MetaSystemHandle::ID(),
    mono_asset_extension::AssetRequestProcessorSystemHandle::ID(),
    mono_scene_extension::SceneSystemHandle::ID(),
    mono_graphics_extension::GraphicsSystemHandle::ID(),
    //SYSTEM_EXECUTION_ORDER_END//
};

} // namespace mono_forge_app_template
