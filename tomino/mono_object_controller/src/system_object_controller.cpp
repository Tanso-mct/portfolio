#include "mono_object_controller/src/pch.h"
#include "mono_object_controller/include/system_object_controller.h"

#pragma comment(lib, "riaecs.lib")
#pragma comment(lib, "mono_identity.lib")
#pragma comment(lib, "mono_transform.lib")
#pragma comment(lib, "mono_input_monitor.lib")
#pragma comment(lib, "mono_d3d12.lib")

#pragma comment(lib, "mono_delta_time.lib")

using namespace DirectX;

#include "mono_object_controller/include/component_object_controller.h"

mono_object_controller::SystemObjectController::SystemObjectController()
{
}

mono_object_controller::SystemObjectController::~SystemObjectController()
{
}

bool mono_object_controller::SystemObjectController::Update
(
    riaecs::IECSWorld &ecsWorld, 
    riaecs::IAssetContainer &assetCont, 
    riaecs::ISystemLoopCommandQueue &systemLoopCmdQueue
){
    // Update delta time
    deltaTimeProvider_.UpdateTime();

    for (const riaecs::Entity &entity : ecsWorld.View(mono_object_controller::ComponentObjectControllerID())())
    {
        // Get identity component
        mono_identity::ComponentIdentity* identity
        = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
            ecsWorld, entity, mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

        // Skip if identity component is not active
        if (!identity->IsActiveSelf())
            continue;

        // Get object controller component
        mono_object_controller::ComponentObjectController* controller
        = riaecs::GetComponent<mono_object_controller::ComponentObjectController>(
            ecsWorld, entity, mono_object_controller::ComponentObjectControllerID());

        riaecs::Entity handlerWindowEntity = controller->GetHandlerWindowEntity();
        if (!handlerWindowEntity.IsValid() || !ecsWorld.HasComponent(handlerWindowEntity, mono_d3d12::ComponentWindowD3D12ID()))
            continue; // No valid window to handle input

        // Get window component
        mono_d3d12::ComponentWindowD3D12* window
        = riaecs::GetComponentWithCheck<mono_d3d12::ComponentWindowD3D12>(
            ecsWorld, handlerWindowEntity, mono_d3d12::ComponentWindowD3D12ID(), "WindowD3D12", RIAECS_LOG_LOC);
            
        // Get input states
        const mono_input_monitor::KeyboardInputState &keyboardState = window->GetKeyboardState();

        // Determine movement direction based on key inputs
        XMFLOAT3 moveDirection = { 0.0f, 0.0f, 0.0f };
        float directionLength = 1.0f;

        if (mono_input_monitor::GetKey(keyboardState, controller->GetForwardKeyCode()))
            moveDirection.z += 1.0f;
        if (mono_input_monitor::GetKey(keyboardState, controller->GetBackKeyCode()))
            moveDirection.z -= 1.0f;
        if (mono_input_monitor::GetKey(keyboardState, controller->GetLeftKeyCode()))
            moveDirection.x -= 1.0f;
        if (mono_input_monitor::GetKey(keyboardState, controller->GetRightKeyCode()))
            moveDirection.x += 1.0f;

        // Normalize xz movement to prevent faster diagonal movement
        if (moveDirection.x != 0.0f || moveDirection.z != 0.0f)
        {
            directionLength = sqrtf(moveDirection.x * moveDirection.x + moveDirection.z * moveDirection.z);
            moveDirection.x /= directionLength;
            moveDirection.z /= directionLength;
        }

        // Store move direction in component
        controller->SetMoveDirection(moveDirection);

        // Apply movement speed
        XMFLOAT3 movement = moveDirection;
        movement.x *= controller->GetMoveSpeed() * deltaTimeProvider_.GetDeltaTime();
        movement.y *= controller->GetMoveSpeed() * deltaTimeProvider_.GetDeltaTime();
        movement.z *= controller->GetMoveSpeed() * deltaTimeProvider_.GetDeltaTime();

        // Get transform component
        mono_transform::ComponentTransform* transform
        = riaecs::GetComponentWithCheck<mono_transform::ComponentTransform>(
            ecsWorld, entity, mono_transform::ComponentTransformID(), "Transform", RIAECS_LOG_LOC);

        // Update position
        XMFLOAT3 currentPos = transform->GetPos();
        currentPos.x += movement.x;
        currentPos.y += movement.y;
        currentPos.z += movement.z;
        transform->SetPos(currentPos, ecsWorld);
    }

    return true; // Continue running
}

MONO_OBJECT_CONTROLLER_API riaecs::SystemFactoryRegistrar
<mono_object_controller::SystemObjectController> mono_object_controller::SystemObjectControllerID;
