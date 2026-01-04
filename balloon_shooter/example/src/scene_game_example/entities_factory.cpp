#include "example/src/pch.h"
#include "example/include/scene_game_example/entities_factory.h"

#include "example/include/mode.h"
#include "example/include/scene_game_example/asset_group.h"

#include "example/include/feature/component_controller.h"

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

void example::GameExampleEntitiesFactory::Create
(
    wb::IAssetContainer &assetCont, 
    wb::IEntityContainer &entityCont, 
    wb::IComponentContainer &componentCont, 
    wb::IEntityIDView &entityIDView
) const
{
#if defined(EXAMPLE_MODE_TRANSFORM)

    // Create a root entity
    std::unique_ptr<wb::IOptionalValue> rootEntityID = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        rootEntityID = entity().GetID().Clone();

        entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
    }

    // Initialize the root entity
    {
        wb::IEntity *entity = entityCont.PtrGet(*rootEntityID);

        wb::IComponent *transformComponent = entity->GetComponent(wbp_transform::TransformComponentID(), componentCont);
        wbp_transform::ITransformComponent *transform = wb::As<wbp_transform::ITransformComponent>(transformComponent);

        transform->SetLocalPosition(DirectX::XMFLOAT3(10.0f, 10.0f, 10.0f));
        transform->SetLocalRotation(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
        transform->SetLocalScale(DirectX::XMFLOAT3(2.0f, 2.0f, 2.0f));
    }

    // Create a child A entity
    std::unique_ptr<wb::IOptionalValue> childAEntityID = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        childAEntityID = entity().GetID().Clone();

        entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
    }

    // Initialize the child A entity
    {
        wb::IEntity *entity = entityCont.PtrGet(*childAEntityID);

        wb::IComponent *transformComponent = entity->GetComponent(wbp_transform::TransformComponentID(), componentCont);
        wbp_transform::ITransformComponent *transform = wb::As<wbp_transform::ITransformComponent>(transformComponent);

        transform->SetLocalPosition(DirectX::XMFLOAT3(5.0f, 0.0f, 0.0f));
        transform->SetLocalRotation(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
        transform->SetLocalScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));

        // Get the root entity
        wb::IEntity *rootEntity = entityCont.PtrGet(*rootEntityID);

        // Set the parent of the child A entity to the root entity
        transform->SetParent(entity, rootEntity, entityCont, componentCont);
    }

    // Create a child B entity
    std::unique_ptr<wb::IOptionalValue> childBEntityID = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        childBEntityID = entity().GetID().Clone();

        entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
    }

    // Initialize the child B entity
    {
        wb::IEntity *entity = entityCont.PtrGet(*childBEntityID);

        wb::IComponent *transformComponent = entity->GetComponent(wbp_transform::TransformComponentID(), componentCont);
        wbp_transform::ITransformComponent *transform = wb::As<wbp_transform::ITransformComponent>(transformComponent);

        transform->SetLocalPosition(DirectX::XMFLOAT3(-5.0f, 0.0f, 0.0f));
        transform->SetLocalRotation(DirectX::XMFLOAT3(0.0f, 90.0f, 0.0f));
        transform->SetLocalScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));

        // Get the root entity
        wb::IEntity *rootEntity = entityCont.PtrGet(*rootEntityID);

        // Set the parent of the child B entity to the root entity
        transform->SetParent(entity, rootEntity, entityCont, componentCont);
    }

    // Create a grandchild entity
    std::unique_ptr<wb::IOptionalValue> grandChildEntityID = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        grandChildEntityID = entity().GetID().Clone();

        entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
    }

    // Initialize the grandchild entity
    {
        wb::IEntity *entity = entityCont.PtrGet(*grandChildEntityID);

        wb::IComponent *transformComponent = entity->GetComponent(wbp_transform::TransformComponentID(), componentCont);
        wbp_transform::ITransformComponent *transform = wb::As<wbp_transform::ITransformComponent>(transformComponent);

        transform->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 5.0f, 0.0f));
        transform->SetLocalRotation(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
        transform->SetLocalScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));

        // Get the child A entity
        wb::IEntity *childAEntity = entityCont.PtrGet(*childAEntityID);

        // Set the parent of the grandchild entity to the child A entity
        transform->SetParent(entity, childAEntity, entityCont, componentCont);
    }

    // Output the explanation
    {
        std::string msg = wb::CreateMessage
        ({
            "[WindowsBasePlugin-Game : wbp_transform]",
            "This example demonstrates the TransformComponent's parent-child relationships.",
            "The root entity has two children: A and B.",
            "Child A has a grandchild entity.",
            "The TransformSystem will update the positions, rotations, and scales based on these relationships."
            "",
            "Root Entity ID: " + std::to_string((*rootEntityID)()),
            "Child A Entity ID: " + std::to_string((*childAEntityID)()),
            "Child B Entity ID: " + std::to_string((*childBEntityID)()),
            "Grandchild Entity ID: " + std::to_string((*grandChildEntityID)()),
            "",
            "Press Space to output the current transform of the entities."
            "Press W, A, S, D to move the root entity.",
        });
        wb::ConsoleLog(msg);
    }

#elif defined(EXAMPLE_MODE_IDENTITY)

    // Create a entity which has IdentityComponent
    std::unique_ptr<wb::IOptionalValue> entityID = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        entityID = entity().GetID().Clone();

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
    }

    // Initialize the entity with IdentityComponent
    {
        wb::IEntity *entity = entityCont.PtrGet(*entityIDView(0).front());

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);

        identity->SetName("Example Entity");
        identity->SetTag(1);
        identity->SetLayer(2);
        identity->SetActiveSelf(true);
    }

    // Output the explanation
    {
        std::string msg = wb::CreateMessage
        ({
            "[WindowsBasePlugin-Game : wbp_identity]",
            "This example demonstrates the IdentityComponent.",
            "An entity with IdentityComponent has been created.",
            "It has a name, tag, layer, and active state.",
            "",
            "Entity ID: " + std::to_string((*entityID)()),
            "",
            "Press Space to output the identity information of the entity."
        });
        wb::ConsoleLog(msg);
    }

#elif defined(EXAMPLE_MODE_FBX_LOADER)

    // Output the explanation
    {
        std::string msg = wb::CreateMessage
        ({
            "[WindowsBasePlugin-Game : wbp_fbx_loader]",
            "This example demonstrates the FBX file loading.",
            "You can see the loaded data to set break points in the debugger.",
            "The file you need to set break points is 'wbp_fbx_loader/src/file_loader_fbx.cpp'.",
        });
        wb::ConsoleLog(msg);
    }

#elif defined(EXAMPLE_MODE_MODEL)
    // Output the explanation
    {
        std::string msg = wb::CreateMessage
        ({
            "[WindowsBasePlugin-Game : wbp_model]",
            "This example demonstrates the ModelAsset loading.",
            "You can see the loaded data to set break points in the debugger.",
            "The file you need to set break points is 'wbp_fbx_loader/src/asset_factory_model.cpp'.",
        });
        wb::ConsoleLog(msg);
    }

#elif defined(EXAMPLE_MODE_RENDER)
    
    // Create a camera entity
    std::unique_ptr<wb::IOptionalValue> cameraEntityID = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        cameraEntityID = entity().GetID().Clone();

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
        entity().AddComponent(wbp_render::CameraComponentID(), componentCont);
    }

    // Initialize the camera entity
    {
        wb::IEntity *entity = entityCont.PtrGet(*cameraEntityID);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("Main Camera");

        wb::IComponent *transformComponent = entity->GetComponent(wbp_transform::TransformComponentID(), componentCont);
        wbp_transform::ITransformComponent *transform = wb::As<wbp_transform::ITransformComponent>(transformComponent);
        transform->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 2.0f, -7.0f));

        wb::IComponent *cameraComponent = entity->GetComponent(wbp_render::CameraComponentID(), componentCont);
        wbp_render::ICameraComponent *camera = wb::As<wbp_render::ICameraComponent>(cameraComponent);
        camera->SetFarZ(10000.0f);
    }

    // Create basic humanoid model entity
    std::unique_ptr<wb::IOptionalValue> modelEntityID = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        modelEntityID = entity().GetID().Clone();

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
        entity().AddComponent(wbp_render::MeshRendererComponentID(), componentCont);
    }

    // Initialize the model entity
    {
        wb::IEntity *entity = entityCont.PtrGet(*modelEntityID);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("Character Model");

        wb::IComponent *transformComponent = entity->GetComponent(wbp_transform::TransformComponentID(), componentCont);
        wbp_transform::ITransformComponent *transform = wb::As<wbp_transform::ITransformComponent>(transformComponent);
        transform->SetLocalRotation(DirectX::XMFLOAT3(0.0f, 90.0f, 0.0f)); // Rotate to face the camera

        wb::IComponent *meshRendererComponent = entity->GetComponent(wbp_render::MeshRendererComponentID(), componentCont);
        wbp_render::IMeshRendererComponent *meshRenderer = wb::As<wbp_render::IMeshRendererComponent>(meshRendererComponent);
        meshRenderer->SetModelAssetID(example::CharacterModelAssetID());
        meshRenderer->AddTextureAssetID(example::CharacterDiffuseTextureAssetID(), wbp_render::TextureType::Diffuse);
        {
            wbp_render::ModelColorConfig colorConfig;
            colorConfig.isColorEnabled = false; // Use texture instead of color
            meshRenderer->SetColorConfig(colorConfig);
        }
    }

    // Create a sprite entity
    std::unique_ptr<wb::IOptionalValue> spriteEntityID = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        spriteEntityID = entity().GetID().Clone();

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::ScreenTransformComponentID(), componentCont);
        entity().AddComponent(wbp_render::SpriteRendererComponentID(), componentCont);
    }
    {
        wb::IEntity *entity = entityCont.PtrGet(*spriteEntityID);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("Sprite");

        wb::IComponent *screenTransformComponent = entity->GetComponent(wbp_transform::ScreenTransformComponentID(), componentCont);
        wbp_transform::IScreenTransformComponent *screenTransform = wb::As<wbp_transform::IScreenTransformComponent>(screenTransformComponent);
        // screenTransform->SetLocalScale(DirectX::XMFLOAT3(300.0f, 300.0f, 1.0f));
        screenTransform->SetLocalPosition(DirectX::XMFLOAT3(-1920.0f / 2, 0.0f, 0.0f));
        // screenTransform->Rotate(DirectX::XMFLOAT3(0.0f, 0.0f, 20.0f));

        screenTransform->TranslateTopLine(300.0f);
        screenTransform->TranslateBottomLine(-300.0f);
        screenTransform->TranslateLeftLine(-300.0f);
        screenTransform->TranslateRightLine(300.0f);

        wb::IComponent *spriteRendererComponent = entity->GetComponent(wbp_render::SpriteRendererComponentID(), componentCont);
        wbp_render::ISpriteRendererComponent *spriteRenderer = wb::As<wbp_render::ISpriteRendererComponent>(spriteRendererComponent);
        {
            wbp_render::SpriteColorConfig colorConfig;
            colorConfig.color = {1.0f, 0.0f, 0.0f, 1.0f};
            colorConfig.isColorEnabled = false; // use texture instead of color
            spriteRenderer->SetColorConfig(colorConfig);
        }
        spriteRenderer->SetTextureAssetID(example::SpriteTextureAssetID());
    }

    // Output the explanation
    {
        std::string msg = wb::CreateMessage
        ({
            "[WindowsBasePlugin-Game : wbp_render]",
            "This example demonstrates the Render.",
            "It render a character model with a camera.",
            "The charater model file is '../resources/example/character.fbx'.",
        });
        wb::ConsoleLog(msg);
    }

#elif defined(EXAMPLE_MODE_COLLISION)

    // Create a field entity
    std::unique_ptr<wb::IOptionalValue> fieldEntityID = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        fieldEntityID = entity().GetID().Clone();

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
        entity().AddComponent(wbp_render::MeshRendererComponentID(), componentCont);
        entity().AddComponent(wbp_collision::BoxColliderComponentID(), componentCont);
    }
    {
        wb::IEntity *entity = entityCont.PtrGet(*fieldEntityID);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("Field");

        wb::IComponent *transformComponent = entity->GetComponent(wbp_transform::TransformComponentID(), componentCont);
        wbp_transform::ITransformComponent *transform = wb::As<wbp_transform::ITransformComponent>(transformComponent);

        wb::IComponent *meshRendererComponent = entity->GetComponent(wbp_render::MeshRendererComponentID(), componentCont);
        wbp_render::IMeshRendererComponent *meshRenderer = wb::As<wbp_render::IMeshRendererComponent>(meshRendererComponent);
        meshRenderer->SetModelAssetID(example::FieldModelAssetID());

        wb::IComponent *boxColliderComponent = entity->GetComponent(wbp_collision::BoxColliderComponentID(), componentCont);
        wbp_collision::IBoxColliderComponent *boxCollider = wb::As<wbp_collision::IBoxColliderComponent>(boxColliderComponent);
        boxCollider->SetColliderShapeAssetID(example::FieldColliderShapeAssetID());
    }

    // Create a controller entity
    std::unique_ptr<wb::IOptionalValue> controllerEntityID = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        controllerEntityID = entity().GetID().Clone();

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
        entity().AddComponent(example::ControllerComponentID(), componentCont);
        entity().AddComponent(wbp_collision::BoxColliderComponentID(), componentCont);
        entity().AddComponent(wbp_collision::RayColliderComponentID(), componentCont);
        entity().AddComponent(wbp_collision::CollisionResultComponentID(), componentCont);
    }

    // Create a camera entity
    std::unique_ptr<wb::IOptionalValue> cameraEntityID = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        cameraEntityID = entity().GetID().Clone();

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
        entity().AddComponent(wbp_render::CameraComponentID(), componentCont);
    }

    // Create a body entity
    std::unique_ptr<wb::IOptionalValue> bodyEntityID = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        bodyEntityID = entity().GetID().Clone();

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
        entity().AddComponent(wbp_render::MeshRendererComponentID(), componentCont);
    }

    // Initialize the controller entity
    {
        wb::IEntity *entity = entityCont.PtrGet(*controllerEntityID);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("Controller");

        wb::IComponent *controllerComponent = entity->GetComponent(example::ControllerComponentID(), componentCont);
        example::IControllerComponent *controller = wb::As<example::IControllerComponent>(controllerComponent);
        
        controller->GetSpeed() = 600.0f;
        controller->GetSensitivity() = 0.3f;

        controller->SetBodyEntityID(bodyEntityID->Clone());
        controller->SetCameraEntityID(cameraEntityID->Clone());

        wb::IComponent *boxColliderComponent = entity->GetComponent(wbp_collision::BoxColliderComponentID(), componentCont);
        wbp_collision::IBoxColliderComponent *boxCollider = wb::As<wbp_collision::IBoxColliderComponent>(boxColliderComponent);
        boxCollider->SetColliderShapeAssetID(example::CharacterColliderShapeAssetID());

        wb::IComponent *rayColliderComponent = entity->GetComponent(wbp_collision::RayColliderComponentID(), componentCont);
        wbp_collision::IRayColliderComponent *rayCollider = wb::As<wbp_collision::IRayColliderComponent>(rayColliderComponent);
        rayCollider->AddTargetComponentID(wbp_collision::BoxColliderComponentID());
    }

    // Initialize the camera entity
    {
        wb::IEntity *entity = entityCont.PtrGet(*cameraEntityID);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("Camera");

        wb::IComponent *transformComponent = entity->GetComponent(wbp_transform::TransformComponentID(), componentCont);
        wbp_transform::ITransformComponent *transform = wb::As<wbp_transform::ITransformComponent>(transformComponent);
        transform->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 110.0f, -260.0f));
        transform->SetLocalRotation(DirectX::XMFLOAT3(30.0f, 0.0f, 0.0f)); // Look down at the body entity

        transform->SetParent(entity, entityCont.PtrGet(*controllerEntityID), entityCont, componentCont);

        wb::IComponent *cameraComponent = entity->GetComponent(wbp_render::CameraComponentID(), componentCont);
        wbp_render::ICameraComponent *camera = wb::As<wbp_render::ICameraComponent>(cameraComponent);
        camera->SetFarZ(100000.0f);
    }

    // Initialize the body entity
    {
        wb::IEntity *entity = entityCont.PtrGet(*bodyEntityID);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("Body");

        wb::IComponent *transformComponent = entity->GetComponent(wbp_transform::TransformComponentID(), componentCont);
        wbp_transform::ITransformComponent *transform = wb::As<wbp_transform::ITransformComponent>(transformComponent);
        transform->SetParent(entity, entityCont.PtrGet(*controllerEntityID), entityCont, componentCont);

        wb::IComponent *meshRendererComponent = entity->GetComponent(wbp_render::MeshRendererComponentID(), componentCont);
        wbp_render::IMeshRendererComponent *meshRenderer = wb::As<wbp_render::IMeshRendererComponent>(meshRendererComponent);
        meshRenderer->SetModelAssetID(example::CharacterModelAssetID());
    }

    // Output the explanation
    {
        std::string msg = wb::CreateMessage
        ({
            "[WindowsBasePlugin-Game : wbp_collision]",
            "This example demonstrates the Collision.",
        });
        wb::ConsoleLog(msg);
    }

#elif defined(EXAMPLE_MODE_PHYSICS)

    // Create a controller entity
    std::unique_ptr<wb::IOptionalValue> controllerEntityID = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        controllerEntityID = entity().GetID().Clone();

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
        entity().AddComponent(example::ControllerComponentID(), componentCont);
        entity().AddComponent(wbp_collision::BoxColliderComponentID(), componentCont);
        entity().AddComponent(wbp_collision::CollisionResultComponentID(), componentCont);
        entity().AddComponent(wbp_physics::RigidBodyComponentID(), componentCont);
    }

    // Create a camera entity
    std::unique_ptr<wb::IOptionalValue> cameraEntityID = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        cameraEntityID = entity().GetID().Clone();

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
        entity().AddComponent(wbp_render::CameraComponentID(), componentCont);
    }

    // Create a body entity
    std::unique_ptr<wb::IOptionalValue> bodyEntityID = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        bodyEntityID = entity().GetID().Clone();

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
        entity().AddComponent(wbp_render::MeshRendererComponentID(), componentCont);
    }

    // Initialize the controller entity
    {
        wb::IEntity *entity = entityCont.PtrGet(*controllerEntityID);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("Controller");

        wb::IComponent *controllerComponent = entity->GetComponent(example::ControllerComponentID(), componentCont);
        example::IControllerComponent *controller = wb::As<example::IControllerComponent>(controllerComponent);
        
        controller->GetSpeed() = 600.0f;
        controller->GetSensitivity() = 0.3f;

        controller->SetBodyEntityID(bodyEntityID->Clone());
        controller->SetCameraEntityID(cameraEntityID->Clone());

        wb::IComponent *boxColliderComponent = entity->GetComponent(wbp_collision::BoxColliderComponentID(), componentCont);
        wbp_collision::IBoxColliderComponent *boxCollider = wb::As<wbp_collision::IBoxColliderComponent>(boxColliderComponent);
        boxCollider->SetColliderShapeAssetID(example::CharacterColliderShapeAssetID());

        wb::IComponent *rigidBodyComponent = entity->GetComponent(wbp_physics::RigidBodyComponentID(), componentCont);
        wbp_physics::IRigidBodyComponent *rigidBody = wb::As<wbp_physics::IRigidBodyComponent>(rigidBodyComponent);
        rigidBody->SetKinematic(true);
    }

    // Initialize the camera entity
    {
        wb::IEntity *entity = entityCont.PtrGet(*cameraEntityID);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("Camera");

        wb::IComponent *transformComponent = entity->GetComponent(wbp_transform::TransformComponentID(), componentCont);
        wbp_transform::ITransformComponent *transform = wb::As<wbp_transform::ITransformComponent>(transformComponent);
        transform->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 110.0f, -260.0f));
        transform->SetLocalRotation(DirectX::XMFLOAT3(30.0f, 0.0f, 0.0f)); // Look down at the body entity

        transform->SetParent(entity, entityCont.PtrGet(*controllerEntityID), entityCont, componentCont);

        wb::IComponent *cameraComponent = entity->GetComponent(wbp_render::CameraComponentID(), componentCont);
        wbp_render::ICameraComponent *camera = wb::As<wbp_render::ICameraComponent>(cameraComponent);
        camera->SetFarZ(100000.0f);
    }

    // Initialize the body entity
    {
        wb::IEntity *entity = entityCont.PtrGet(*bodyEntityID);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("Body");

        wb::IComponent *transformComponent = entity->GetComponent(wbp_transform::TransformComponentID(), componentCont);
        wbp_transform::ITransformComponent *transform = wb::As<wbp_transform::ITransformComponent>(transformComponent);
        transform->SetParent(entity, entityCont.PtrGet(*controllerEntityID), entityCont, componentCont);

        wb::IComponent *meshRendererComponent = entity->GetComponent(wbp_render::MeshRendererComponentID(), componentCont);
        wbp_render::IMeshRendererComponent *meshRenderer = wb::As<wbp_render::IMeshRendererComponent>(meshRendererComponent);
        meshRenderer->SetModelAssetID(example::CharacterModelAssetID());
    }

    // Create a field entity
    std::unique_ptr<wb::IOptionalValue> fieldEntityID = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        fieldEntityID = entity().GetID().Clone();

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
        entity().AddComponent(wbp_render::MeshRendererComponentID(), componentCont);
        entity().AddComponent(wbp_collision::BoxColliderComponentID(), componentCont);
    }
    {
        wb::IEntity *entity = entityCont.PtrGet(*fieldEntityID);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("Field");

        wb::IComponent *transformComponent = entity->GetComponent(wbp_transform::TransformComponentID(), componentCont);
        wbp_transform::ITransformComponent *transform = wb::As<wbp_transform::ITransformComponent>(transformComponent);

        wb::IComponent *meshRendererComponent = entity->GetComponent(wbp_render::MeshRendererComponentID(), componentCont);
        wbp_render::IMeshRendererComponent *meshRenderer = wb::As<wbp_render::IMeshRendererComponent>(meshRendererComponent);
        meshRenderer->SetModelAssetID(example::FieldModelAssetID());

        wb::IComponent *boxColliderComponent = entity->GetComponent(wbp_collision::BoxColliderComponentID(), componentCont);
        wbp_collision::IBoxColliderComponent *boxCollider = wb::As<wbp_collision::IBoxColliderComponent>(boxColliderComponent);
        boxCollider->SetColliderShapeAssetID(example::FieldColliderShapeAssetID());
    }

    // Output the explanation
    {
        std::string msg = wb::CreateMessage
        ({
            "[WindowsBasePlugin-Game : wbp_physics]",
            "This example demonstrates the Physics.",
            "It simulates the physics of the character model and the field.",
            "The character model can move and collide with the field.",
        });
        wb::ConsoleLog(msg);
    }

#elif defined(EXAMPLE_MODE_LOCATOR)

    // Output the explanation
    {
        std::string msg = wb::CreateMessage
        ({
            "[WindowsBasePlugin-Game : wbp_locator]",
            "This example demonstrates the Locator.",
            "It provides a way to locate assets in the project.",
            "You can see the loaded data to set break points in the debugger.",
            "The file you need to set break points is 'wbp_locator/src/asset_factory_locator.cpp'.",
        });
        wb::ConsoleLog(msg);
    }

    // Create a controller entity
    std::unique_ptr<wb::IOptionalValue> controllerEntityID = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        controllerEntityID = entity().GetID().Clone();

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
        entity().AddComponent(example::ControllerComponentID(), componentCont);
        entity().AddComponent(wbp_locator::LocatorComponentID(), componentCont);
    }

    // Create a camera entity
    std::unique_ptr<wb::IOptionalValue> cameraEntityID = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        cameraEntityID = entity().GetID().Clone();

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
        entity().AddComponent(wbp_render::CameraComponentID(), componentCont);
    }

    // Create a body entity
    std::unique_ptr<wb::IOptionalValue> bodyEntityID = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        bodyEntityID = entity().GetID().Clone();

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
        entity().AddComponent(wbp_render::MeshRendererComponentID(), componentCont);
    }

    // Initialize the controller entity
    {
        wb::IEntity *entity = entityCont.PtrGet(*controllerEntityID);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("Controller");

        wb::IComponent *controllerComponent = entity->GetComponent(example::ControllerComponentID(), componentCont);
        example::IControllerComponent *controller = wb::As<example::IControllerComponent>(controllerComponent);
        
        controller->GetSpeed() = 600.0f;
        controller->GetSensitivity() = 0.3f;

        controller->SetBodyEntityID(bodyEntityID->Clone());
        controller->SetCameraEntityID(cameraEntityID->Clone());

        wb::IComponent *locatorComponent = entity->GetComponent(wbp_locator::LocatorComponentID(), componentCont);
        wbp_locator::ILocatorComponent *locator = wb::As<wbp_locator::ILocatorComponent>(locatorComponent);
        locator->SetLocatorAssetID(example::CharacterLocatorAssetID());

        // Currently use locator asset, it enable to set 0 or 1.
        locator->SetLocateTargetIndex(1);
    }

    // Initialize the camera entity
    {
        wb::IEntity *entity = entityCont.PtrGet(*cameraEntityID);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("Camera");

        wb::IComponent *transformComponent = entity->GetComponent(wbp_transform::TransformComponentID(), componentCont);
        wbp_transform::ITransformComponent *transform = wb::As<wbp_transform::ITransformComponent>(transformComponent);
        transform->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 110.0f, -260.0f));
        transform->SetLocalRotation(DirectX::XMFLOAT3(30.0f, 0.0f, 0.0f)); // Look down at the body entity

        transform->SetParent(entity, entityCont.PtrGet(*controllerEntityID), entityCont, componentCont);

        wb::IComponent *cameraComponent = entity->GetComponent(wbp_render::CameraComponentID(), componentCont);
        wbp_render::ICameraComponent *camera = wb::As<wbp_render::ICameraComponent>(cameraComponent);
        camera->SetFarZ(100000.0f);
    }

    // Initialize the body entity
    {
        wb::IEntity *entity = entityCont.PtrGet(*bodyEntityID);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("Body");

        wb::IComponent *transformComponent = entity->GetComponent(wbp_transform::TransformComponentID(), componentCont);
        wbp_transform::ITransformComponent *transform = wb::As<wbp_transform::ITransformComponent>(transformComponent);
        transform->SetParent(entity, entityCont.PtrGet(*controllerEntityID), entityCont, componentCont);

        wb::IComponent *meshRendererComponent = entity->GetComponent(wbp_render::MeshRendererComponentID(), componentCont);
        wbp_render::IMeshRendererComponent *meshRenderer = wb::As<wbp_render::IMeshRendererComponent>(meshRendererComponent);
        meshRenderer->SetModelAssetID(example::CharacterModelAssetID());
    }

    // Create a field entity
    std::unique_ptr<wb::IOptionalValue> fieldEntityID = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        fieldEntityID = entity().GetID().Clone();

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
        entity().AddComponent(wbp_render::MeshRendererComponentID(), componentCont);
    }
    {
        wb::IEntity *entity = entityCont.PtrGet(*fieldEntityID);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("Field");

        wb::IComponent *transformComponent = entity->GetComponent(wbp_transform::TransformComponentID(), componentCont);
        wbp_transform::ITransformComponent *transform = wb::As<wbp_transform::ITransformComponent>(transformComponent);

        wb::IComponent *meshRendererComponent = entity->GetComponent(wbp_render::MeshRendererComponentID(), componentCont);
        wbp_render::IMeshRendererComponent *meshRenderer = wb::As<wbp_render::IMeshRendererComponent>(meshRendererComponent);
        meshRenderer->SetModelAssetID(example::FieldModelAssetID());
    }

#else
    std::string msg = wb::CreateMessage
    ({
        "[WindowsBasePlugin-Game]",
        "This is the example application for the WindowsBasePlugin-Game.",
        "There are modes available to demonstrate various features.",
        "Please define one of the modes in 'example/include/mode.h'."
    });
    wb::ConsoleLog(msg);

#endif
}