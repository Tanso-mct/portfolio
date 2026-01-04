#include "example/src/pch.h"
#include "example/include/scene_game_example/system_scheduler.h"

#include "example/include/mode.h"

#include "example/include/feature/system_controller.h"

#include "wbp_transform/plugin.h"
#pragma comment(lib, "wbp_transform.lib")

#include "wbp_identity/plugin.h"
#pragma comment(lib, "wbp_identity.lib")

#include "wbp_render/plugin.h"
#pragma comment(lib, "wbp_render.lib")

#include "wbp_collision/plugin.h"
#pragma comment(lib, "wbp_collision.lib")

#include "wbp_physics/plugin.h"
#pragma comment(lib, "wbp_physics.lib")

#include "wbp_locator/plugin.h"
#pragma comment(lib, "wbp_locator.lib")

using namespace DirectX;

void example::GameExampleSystemScheduler::Execute(wb::ISystemContainer &systemCont, wb::SystemArgument &args)
{
    systemCont.Get(wbp_transform::TransformSystemID()).Update(args);

#if defined(EXAMPLE_MODE_TRANSFORM)

    // Get containers to use
    wb::IWindowContainer &windowContainer = args.containerStorage_.GetContainer<wb::IWindowContainer>();
    wb::IMonitorContainer &monitorContainer = args.containerStorage_.GetContainer<wb::IMonitorContainer>();
    wb::IAssetContainer &assetContainer = args.containerStorage_.GetContainer<wb::IAssetContainer>();

    // Get the window facade for the current window
    wb::IWindowFacade &window = windowContainer.Get(args.belongWindowID_);

    // Get the keyboard and mouse monitors
    wb::IKeyboardMonitor *keyboardMonitor = nullptr;
    wb::IMouseMonitor *mouseMonitor = nullptr;
    for (const size_t &monitorID : window.GetMonitorIDs())
    {
        wb::IMonitor &monitor = monitorContainer.Get(monitorID);
        if (keyboardMonitor == nullptr) keyboardMonitor = wb::As<wb::IKeyboardMonitor>(&monitor);
        if (mouseMonitor == nullptr) mouseMonitor = wb::As<wb::IMouseMonitor>(&monitor);
    }

    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(wbp_transform::TransformComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue; // Skip if entity is null

        wb::IComponent *transformComponent = entity->GetComponent(wbp_transform::TransformComponentID(), args.componentContainer_);
        wbp_transform::ITransformComponent *transform = wb::As<wbp_transform::ITransformComponent>(transformComponent);

        if (keyboardMonitor != nullptr && keyboardMonitor->GetKeyDown(wb::KeyCode::Space))
        {
            std::string msg = wb::CreateMessage
            ({
                "Entity ID: " + std::to_string(entity->GetID()()),
                "World Position: " +
                std::to_string(transform->GetPosition().x) + ", " + 
                std::to_string(transform->GetPosition().y) + ", " + 
                std::to_string(transform->GetPosition().z) +
                " World Rotation: " +
                std::to_string(transform->GetQuatRotation().x) + ", " +
                std::to_string(transform->GetQuatRotation().y) + ", " +
                std::to_string(transform->GetQuatRotation().z) + ", " +
                " World Scale: " +
                std::to_string(transform->GetScale().x) + ", " +
                std::to_string(transform->GetScale().y) + ", " +
                std::to_string(transform->GetScale().z),
                "Local Position: " +
                std::to_string(transform->GetLocalPosition().x) + ", " +
                std::to_string(transform->GetLocalPosition().y) + ", " +
                std::to_string(transform->GetLocalPosition().z) +
                " Local Rotation: " +
                std::to_string(transform->GetQuatLocalRotation().x) + ", " +
                std::to_string(transform->GetQuatLocalRotation().y) + ", " +
                std::to_string(transform->GetQuatLocalRotation().z) + ", " +
                " Local Scale: " +
                std::to_string(transform->GetLocalScale().x) + ", " +
                std::to_string(transform->GetLocalScale().y) + ", " +
                std::to_string(transform->GetLocalScale().z)
            });
            wb::ConsoleLog(msg);
        }
        else if (keyboardMonitor != nullptr)
        {
            if (transform->GetParent(args.entityContainer_) != nullptr) continue;

            // Create move axis based on keyboard input
            XMFLOAT3 moveAxis(0.0f, 0.0f, 0.0f);
            if (keyboardMonitor->GetKeyDown(wb::KeyCode::W)) moveAxis.z += 1.0f;
            if (keyboardMonitor->GetKeyDown(wb::KeyCode::S)) moveAxis.z -= 1.0f;
            if (keyboardMonitor->GetKeyDown(wb::KeyCode::A)) moveAxis.x -= 1.0f;
            if (keyboardMonitor->GetKeyDown(wb::KeyCode::D)) moveAxis.x += 1.0f;

            if (moveAxis.x == 0.0f && moveAxis.y == 0.0f && moveAxis.z == 0.0f)
            {
                continue; // Skip if no movement input
            }

            // Move the root entity based on keyboard input
            transform->Translate(moveAxis);

            // Log the movement values
            std::string msg = wb::CreateMessage
            ({
                "Root entity Moved by: " +
                std::to_string(moveAxis.x) + ", " +
                std::to_string(moveAxis.y) + ", " +
                std::to_string(moveAxis.z)
            });
            wb::ConsoleLog(msg);

        }
    }

#elif defined(EXAMPLE_MODE_IDENTITY)

    // Get containers to use
    wb::IWindowContainer &windowContainer = args.containerStorage_.GetContainer<wb::IWindowContainer>();
    wb::IMonitorContainer &monitorContainer = args.containerStorage_.GetContainer<wb::IMonitorContainer>();
    wb::IAssetContainer &assetContainer = args.containerStorage_.GetContainer<wb::IAssetContainer>();

    // Get the window facade for the current window
    wb::IWindowFacade &window = windowContainer.Get(args.belongWindowID_);

    // Get the keyboard and mouse monitors
    wb::IKeyboardMonitor *keyboardMonitor = nullptr;
    wb::IMouseMonitor *mouseMonitor = nullptr;
    for (const size_t &monitorID : window.GetMonitorIDs())
    {
        wb::IMonitor &monitor = monitorContainer.Get(monitorID);
        if (keyboardMonitor == nullptr) keyboardMonitor = wb::As<wb::IKeyboardMonitor>(&monitor);
        if (mouseMonitor == nullptr) mouseMonitor = wb::As<wb::IMouseMonitor>(&monitor);
    }

    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(wbp_identity::IdentityComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue; // Skip if entity is null

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), args.componentContainer_);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);

        if (keyboardMonitor != nullptr && keyboardMonitor->GetKeyDown(wb::KeyCode::Space))
        {
            std::string msg = wb::CreateMessage
            ({
                "Name: " + std::string(identity->GetName()),
                "Tag: " + std::to_string(identity->GetTag()),
                "Layer: " + std::to_string(identity->GetLayer()),
                "Active Self: " + std::to_string(identity->IsActiveSelf())
            });
            wb::ConsoleLog(msg);
        }
    }

#elif defined(EXAMPLE_MODE_RENDER)

    systemCont.Get(wbp_transform::TransformSystemID()).Update(args);
    systemCont.Get(wbp_render::RenderSystemID()).Update(args);

#elif defined(EXAMPLE_MODE_COLLISION)

    systemCont.Get(example::ControllerSystemID()).Update(args);
    systemCont.Get(wbp_transform::TransformSystemID()).Update(args);
    systemCont.Get(wbp_collision::CollisionSystemID()).Update(args);
    systemCont.Get(wbp_render::RenderSystemID()).Update(args);

#elif defined(EXAMPLE_MODE_PHYSICS)

    systemCont.Get(example::ControllerSystemID()).Update(args);
    systemCont.Get(wbp_physics::RigidBodySystemID()).Update(args);
    systemCont.Get(wbp_transform::TransformSystemID()).Update(args);

    systemCont.Get(wbp_collision::CollisionSystemID()).Update(args);
    systemCont.Get(wbp_physics::RigidBodyResponseSystemID()).Update(args);

    systemCont.Get(wbp_render::RenderSystemID()).Update(args);

#elif defined(EXAMPLE_MODE_LOCATOR)

    static bool isFirstUpdate = true;

    if (isFirstUpdate)
    {
        systemCont.Get(wbp_locator::LocatorSystemID()).Update(args);
        systemCont.Get(wbp_transform::TransformSystemID()).Update(args);

        isFirstUpdate = false;
        return;
    }
    
    systemCont.Get(example::ControllerSystemID()).Update(args);
    systemCont.Get(wbp_transform::TransformSystemID()).Update(args);

    systemCont.Get(wbp_render::RenderSystemID()).Update(args);
#endif
}