#include "mono_entity_archive_extension/src/pch.h"
#include "mono_entity_archive_extension/include/transform_manipulate_system.h"

using namespace DirectX;

#include "mono_meta_extension/include/meta_component.h"
#include "mono_transform_extension/include/transform_component.h"
#include "mono_scene_extension/include/scene_tag_component.h"
#include "mono_graphics_extension/include/camera_component.h"
#include "mono_graphics_extension/include/ui_component.h"
#include "mono_graphics_extension/include/graphics_system.h"

#include "mono_entity_archive_extension/include/inspector_ui_component.h"
#include "mono_entity_archive_extension/include/inspector_drawer.h"
#include "mono_entity_archive_extension/include/transform_manipulator_ui_component.h"
#include "mono_entity_archive_extension/include/manipulator_drawer.h"
#include "mono_entity_archive_service/include/entity_archive_service_command_list.h"

namespace mono_entity_archive_extension
{

void XMQuaternionToEulerAngles(const XMVECTOR& q, float& pitch, float& yaw, float& roll)
{
    XMMATRIX m = XMMatrixRotationQuaternion(q);

    // Decomposition of Euler angles (right-hand system, Yaw=Y, Pitch=X, Roll=Z, YXZ order)

    // Pitch
    pitch = asinf(-m.r[2].m128_f32[1]); // -m32

    // Yaw, Roll
    if (cosf(pitch) > 1e-6) 
    {
        yaw  = atan2f(m.r[2].m128_f32[0], m.r[2].m128_f32[2]); // m31, m33
        roll = atan2f(m.r[0].m128_f32[1], m.r[1].m128_f32[1]); // m12, m22
    } 
    else 
    {
        // When gimbal locked
        yaw  = atan2f(-m.r[0].m128_f32[2], m.r[0].m128_f32[0]);
        roll = 0.0f;
    }
}

TransformManipulateSystem::TransformManipulateSystem(std::unique_ptr<mono_service::ServiceProxy> entity_archive_service_proxy_) :
    entity_archive_service_proxy_(std::move(entity_archive_service_proxy_))
{
}

TransformManipulateSystem::~TransformManipulateSystem()
{
}

bool TransformManipulateSystem::PreUpdate(ecs::World& world)
{
    return true; // Success
}

bool TransformManipulateSystem::Update(ecs::World& world)
{
    // Structure to hold manipulator info
    struct ManipulatorInfo
    {
        ecs::Entity manipulator_entity;
        ecs::Entity camera_entity;
    };

    // Map from scene entity to its manipulator info
    std::unordered_map<mono_scene_extension::SceneID, ManipulatorInfo> scene_to_manip_info_map;

    // Iterate through all entities with TransformManipulatorUIComponent
    for (const ecs::Entity& entity : world.View(TransformManipulatorUIComponentHandle::ID())())
    {
        // Get meta component
        mono_meta_extension::MetaComponent* meta_component
            = world.GetComponent<mono_meta_extension::MetaComponent>(
                entity, mono_meta_extension::MetaComponentHandle::ID());
        assert(meta_component != nullptr && "Entity with TransformManipulatorUIComponent must have MetaComponent!");

        if (!meta_component->IsActiveSelf())
            continue; // Skip if the entity is not active

        // Get scene tag component
        mono_scene_extension::SceneTagComponent* scene_tag_component
            = world.GetComponent<mono_scene_extension::SceneTagComponent>(
                entity, mono_scene_extension::SceneTagComponentHandle::ID());
        assert(scene_tag_component != nullptr && "Entity with TransformManipulatorUIComponent must have SceneTagComponent!");

        // Store the mapping from scene entity to manipulator entity
        scene_to_manip_info_map[scene_tag_component->GetSceneID()].manipulator_entity = entity;

        // Get ui component
        mono_graphics_extension::UIComponent* ui_component
            = world.GetComponent<mono_graphics_extension::UIComponent>(
                entity, mono_graphics_extension::UIComponentHandle::ID());
        assert(ui_component != nullptr && "Entity with TransformManipulatorUIComponent must have UIComponent!");

        // Get manipulator drawer
        ManipulatorDrawer* manipulator_drawer
            = dynamic_cast<ManipulatorDrawer*>(&ui_component->GetDrawer());
        assert(manipulator_drawer != nullptr && "UIComponent's drawer must be ManipulatorDrawer!");

        if (manipulator_drawer->IsEdited())
        {
            // Get the editing entity
            ecs::Entity editing_entity = manipulator_drawer->GetEditingEntity();

            // Get transform component of the editing entity
            mono_transform_extension::TransformComponent* transform_component
                = world.GetComponent<mono_transform_extension::TransformComponent>(
                    editing_entity, mono_transform_extension::TransformComponentHandle::ID());
            assert(transform_component != nullptr && "Editing entity must have TransformComponent!");

            // Get object matrix
            XMFLOAT4X4 object_matrix = manipulator_drawer->GetObjectMatrix();

            // Decompose matrix
            XMVECTOR scale_vec;
            XMVECTOR rotation_vec;
            XMVECTOR translation_vec;
            XMMATRIX object_xmmatrix = XMLoadFloat4x4(&object_matrix);
            XMMatrixDecompose(&scale_vec, &rotation_vec, &translation_vec, object_xmmatrix);

            // Store new TRS
            XMFLOAT3 new_position;
            XMFLOAT4 new_rotation;
            XMFLOAT3 new_scale;
            XMStoreFloat3(&new_position, translation_vec);
            XMStoreFloat4(&new_rotation, rotation_vec);
            XMStoreFloat3(&new_scale, scale_vec);

            // Update transform component
            transform_component->SetWorldPosition(new_position);
            transform_component->SetWorldRotation(new_rotation);
            transform_component->SetWorldScale(new_scale);

            // Convert rotation to Euler angles
            float pitch, yaw, roll;
            XMQuaternionToEulerAngles(rotation_vec, pitch, yaw, roll);

            // Get entity archive service command list
            std::unique_ptr<mono_service::ServiceCommandList> command_list 
                = entity_archive_service_proxy_->CreateCommandList();
            mono_entity_archive_service::EntityArchiveServiceCommandList* entity_archive_command_list
                = dynamic_cast<mono_entity_archive_service::EntityArchiveServiceCommandList*>(command_list.get());
            assert(entity_archive_command_list != nullptr && "Failed to create EntityArchiveServiceCommandList");

            // Edit setup param in entity archive service
            std::unique_ptr<mono_transform_extension::TransformComponent::SetupParam> new_setup_param
                = std::make_unique<mono_transform_extension::TransformComponent::SetupParam>();
            new_setup_param->position = new_position;
            new_setup_param->rotation = XMFLOAT3(
                XMConvertToDegrees(pitch),
                XMConvertToDegrees(yaw),
                XMConvertToDegrees(roll));
            new_setup_param->scale = new_scale;

            entity_archive_command_list->EditSetupParam(
                editing_entity, mono_transform_extension::TransformComponentHandle::ID(),
                std::move(new_setup_param));

            // Submit command list
            entity_archive_service_proxy_->SubmitCommandList(std::move(command_list));
        }
    }

    // Map from scene entity to its camera entity
    std::unordered_map<mono_scene_extension::SceneID, ecs::Entity> scene_to_camera_entity_map;

    // Iterate through all entities with CameraComponent
    for (const ecs::Entity& entity : world.View(mono_graphics_extension::CameraComponentHandle::ID())())
    {
        // Get meta component
        mono_meta_extension::MetaComponent* meta_component
            = world.GetComponent<mono_meta_extension::MetaComponent>(
                entity, mono_meta_extension::MetaComponentHandle::ID());
        assert(meta_component != nullptr && "Entity with CameraComponent must have MetaComponent!");

        if (!meta_component->IsActiveSelf())
            continue; // Skip if the entity is not active

        // Get scene tag component
        mono_scene_extension::SceneTagComponent* scene_tag_component
            = world.GetComponent<mono_scene_extension::SceneTagComponent>(
                entity, mono_scene_extension::SceneTagComponentHandle::ID());
        assert(scene_tag_component != nullptr && "Entity with CameraComponent must have SceneTagComponent!");

        // Check if there is a manipulator entity for this scene
        auto it = scene_to_manip_info_map.find(scene_tag_component->GetSceneID());
        if (it == scene_to_manip_info_map.end())
            continue; // No manipulator entity for this scene

        // Store the camera entity in the manipulator info
        it->second.camera_entity = entity;
    }

    // Iterate through all entities with InspectorUIComponent
    for (const ecs::Entity& entity : world.View(InspectorUIComponentHandle::ID())())
    {
        // Get meta component
        mono_meta_extension::MetaComponent* meta_component
            = world.GetComponent<mono_meta_extension::MetaComponent>(
                entity, mono_meta_extension::MetaComponentHandle::ID());
        assert(meta_component != nullptr && "Entity with InspectorUIComponent must have MetaComponent!");

        if (!meta_component->IsActiveSelf())
            continue; // Skip if the entity is not active

        // Get scene tag component
        mono_scene_extension::SceneTagComponent* scene_tag_component
            = world.GetComponent<mono_scene_extension::SceneTagComponent>(
                entity, mono_scene_extension::SceneTagComponentHandle::ID());
        assert(scene_tag_component != nullptr && "Entity with InspectorUIComponent must have SceneTagComponent!");

        // Check if there is a manipulator entity for this scene
        auto it = scene_to_manip_info_map.find(scene_tag_component->GetSceneID());
        if (it == scene_to_manip_info_map.end())
            continue; // No manipulator entity for this scene

        // Get the manipulator info
        ManipulatorInfo& manip_info = it->second;

        // Get UI component from inspector entity
        mono_graphics_extension::UIComponent* inspec_ui_component
            = world.GetComponent<mono_graphics_extension::UIComponent>(
                entity, mono_graphics_extension::UIComponentHandle::ID());
        assert(inspec_ui_component != nullptr && "Entity with InspectorUIComponent must have UIComponent!");

        // Get inspector drawer
        InspectorDrawer* inspector_drawer
            = dynamic_cast<InspectorDrawer*>(&inspec_ui_component->GetDrawer());
        assert(inspector_drawer != nullptr && "UIComponent's drawer must be InspectorDrawer!");

        // Get inspected entity
        ecs::Entity inspected_entity = inspector_drawer->GetInspectedEntity();
        if (inspected_entity == ecs::Entity())
            continue; // Skip if no inspected entity

        // Check if the inspected entity has TransformComponent
        if (!world.HasComponent(inspected_entity, mono_transform_extension::TransformComponentHandle::ID()))
            continue; // Skip if no TransformComponent

        // Get UI component from manipulator entity
        mono_graphics_extension::UIComponent* manip_ui_component
            = world.GetComponent<mono_graphics_extension::UIComponent>(
                manip_info.manipulator_entity, mono_graphics_extension::UIComponentHandle::ID());
        assert(manip_ui_component != nullptr && "Entity with TransformManipulatorUIComponent must have UIComponent!");

        // Get manipulator drawer
        ManipulatorDrawer* manipulator_drawer
            = dynamic_cast<ManipulatorDrawer*>(&manip_ui_component->GetDrawer());
        assert(manipulator_drawer != nullptr && "UIComponent's drawer must be ManipulatorDrawer!");

        // Set editing entity
        manipulator_drawer->SetEditingEntity(inspected_entity);

        // Get camera component of camera entity
        mono_graphics_extension::CameraComponent* camera_component
            = world.GetComponent<mono_graphics_extension::CameraComponent>(
                manip_info.camera_entity, mono_graphics_extension::CameraComponentHandle::ID());
        assert(camera_component != nullptr && "Camera entity must have CameraComponent!");

        // Get transform component of camera entity
        mono_transform_extension::TransformComponent* camera_transform_component
            = world.GetComponent<mono_transform_extension::TransformComponent>(
                manip_info.camera_entity, mono_transform_extension::TransformComponentHandle::ID());
        assert(camera_transform_component != nullptr && "Camera entity must have TransformComponent!");

        // Get trs
        XMFLOAT3 translation = camera_transform_component->GetWorldPosition();
        XMFLOAT4 rotation = camera_transform_component->GetWorldRotation();

        // Calculate view matrix
        XMVECTOR up_vec = XMVectorSet(
            mono_graphics_extension::UP_VECTOR.x, 
            mono_graphics_extension::UP_VECTOR.y, 
            mono_graphics_extension::UP_VECTOR.z, 
            mono_graphics_extension::UP_VECTOR.w);
        XMVECTOR forward_vec = XMVectorSet(
            mono_graphics_extension::FORWARD_VECTOR.x, 
            mono_graphics_extension::FORWARD_VECTOR.y, 
            mono_graphics_extension::FORWARD_VECTOR.z, 
            mono_graphics_extension::FORWARD_VECTOR.w);
        XMVECTOR rot_vec = XMLoadFloat4(&rotation);
        XMVECTOR rotated_forward_vec = XMVector3Rotate(forward_vec, rot_vec);
        XMVECTOR rotated_up_vec = XMVector3Rotate(up_vec, rot_vec);
        XMVECTOR position_vec = XMLoadFloat3(&translation);

        XMMATRIX view_matrix = XMMatrixLookAtLH(
            position_vec, XMVectorAdd(position_vec, rotated_forward_vec), rotated_up_vec);

        // Set view matrix to manipulator drawer
        XMFLOAT4X4 view_matrix_float4x4;
        XMStoreFloat4x4(&view_matrix_float4x4, view_matrix);
        manipulator_drawer->SetViewMatrix(view_matrix_float4x4);

        // Calculate projection matrix
        XMMATRIX projection_matrix = XMMatrixPerspectiveFovLH(
            camera_component->GetFovY(), camera_component->GetAspectRatio(), 
            camera_component->GetNearZ(), camera_component->GetFarZ());

        // Set projection matrix to manipulator drawer
        XMFLOAT4X4 projection_matrix_float4x4;
        XMStoreFloat4x4(&projection_matrix_float4x4, projection_matrix);
        manipulator_drawer->SetProjectionMatrix(projection_matrix_float4x4);

        // Get transform component of inspected entity
        mono_transform_extension::TransformComponent* inspected_transform_component
            = world.GetComponent<mono_transform_extension::TransformComponent>(
                inspected_entity, mono_transform_extension::TransformComponentHandle::ID());
        assert(inspected_transform_component != nullptr && "Inspected entity must have TransformComponent!");

        // Get object matrix
        XMMATRIX object_matrix = inspected_transform_component->GetWorldMatrix();
        XMFLOAT4X4 object_matrix_float4x4;
        XMStoreFloat4x4(&object_matrix_float4x4, object_matrix);

        // Set object matrix to manipulator drawer
        manipulator_drawer->SetObjectMatrix(object_matrix_float4x4);
    }

    return true; // Success
}

bool TransformManipulateSystem::PostUpdate(ecs::World& world)
{
    return true; // Success
}

ecs::SystemID TransformManipulateSystem::GetID() const
{
    return TransformManipulateSystemHandle::ID();
}

} // namespace mono_entity_archive_extension