#include "mono_transform_test/pch.h"

#pragma comment(lib, "riaecs.lib")

#include "mem_alloc_fixed_block/mem_alloc_fixed_block.h"
#pragma comment(lib, "mem_alloc_fixed_block.lib")

#include "mono_transform/include/component_transform.h"
#pragma comment(lib, "mono_transform.lib")

using namespace DirectX;

TEST(Transform, Initialize)
{
    mono_transform::ComponentTransform transform;
    mono_transform::ComponentTransform::SetupParam param;
    param.pos_ = DirectX::XMFLOAT3(1.0f, 2.0f, 3.0f);
    transform.Setup(param);

    EXPECT_EQ(transform.IsInitialized(), true);
}

TEST(Transform, Position)
{
    std::unique_ptr<riaecs::IECSWorld> ecsWorld = std::make_unique<riaecs::ECSWorld>(*riaecs::gComponentFactoryRegistry, *riaecs::gComponentMaxCountRegistry);
    ecsWorld->SetPoolFactory(std::make_unique<mem_alloc_fixed_block::FixedBlockPoolFactory>());
    ecsWorld->SetAllocatorFactory(std::make_unique<mem_alloc_fixed_block::FixedBlockAllocatorFactory>());
    ecsWorld->CreateWorld();

    // Create entity
    riaecs::Entity entity = ecsWorld->CreateEntity();
    ecsWorld->AddComponent(entity, mono_transform::ComponentTransformID());
    {
        mono_transform::ComponentTransform* transform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, entity, mono_transform::ComponentTransformID());

        mono_transform::ComponentTransform::SetupParam param;
        param.pos_ = DirectX::XMFLOAT3(10.0f, 0.0f, 0.0f);
        transform->Setup(param);

        // Set world position
        transform->SetPos(XMFLOAT3(20.0f, 0.0f, 0.0f), *ecsWorld);
        EXPECT_FLOAT_EQ(transform->GetPos().x, 20.0f);

        // Set local position (no parent, so world position should be same as local)
        transform->SetLocalPos(XMFLOAT3(30.0f, 0.0f, 0.0f), *ecsWorld);
        EXPECT_FLOAT_EQ(transform->GetPos().x, 20.0f);
        EXPECT_FLOAT_EQ(transform->GetLocalPos().x, 30.0f);
    }
}

TEST(Transform, Rotation)
{
    std::unique_ptr<riaecs::IECSWorld> ecsWorld = std::make_unique<riaecs::ECSWorld>(*riaecs::gComponentFactoryRegistry, *riaecs::gComponentMaxCountRegistry);
    ecsWorld->SetPoolFactory(std::make_unique<mem_alloc_fixed_block::FixedBlockPoolFactory>());
    ecsWorld->SetAllocatorFactory(std::make_unique<mem_alloc_fixed_block::FixedBlockAllocatorFactory>());
    ecsWorld->CreateWorld();

    const float pitch = 30.0f; // degrees
    const float yaw = 45.0f;   // degrees
    const float roll = 60.0f;  // degrees

    // Create entity
    riaecs::Entity entity = ecsWorld->CreateEntity();
    ecsWorld->AddComponent(entity, mono_transform::ComponentTransformID());
    {
        mono_transform::ComponentTransform* transform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, entity, mono_transform::ComponentTransformID());

        mono_transform::ComponentTransform::SetupParam param;
        param.pos_ = DirectX::XMFLOAT3(10.0f, 0.0f, 0.0f);
        transform->Setup(param);

        // Set rotation from Euler angles
        transform->SetRotFromEuler(pitch, yaw, roll, *ecsWorld);

        // Get Euler angles from rotation
        float pitchOut, yawOut, rollOut;
        XMFLOAT3 euler = transform->GetRotByEuler();
        EXPECT_FLOAT_EQ(pitch, euler.x);
        EXPECT_FLOAT_EQ(yaw, euler.y);
        EXPECT_FLOAT_EQ(roll, euler.z);

        // Set local rotation from Euler angles (no parent, so world rotation should be same as local)
        transform->SetLocalRotFromEuler (pitch * 2.0f, yaw * 2.0f, roll * 2.0f, *ecsWorld);
        euler = transform->GetLocalRotByEuler();
        EXPECT_FLOAT_EQ(pitch * 2.0f, euler.x);
        EXPECT_FLOAT_EQ(yaw * 2.0f, euler.y);
        EXPECT_FLOAT_EQ(roll * 2.0f, euler.z);
    }
}

TEST(Transform, Scale)
{
    std::unique_ptr<riaecs::IECSWorld> ecsWorld = std::make_unique<riaecs::ECSWorld>(*riaecs::gComponentFactoryRegistry, *riaecs::gComponentMaxCountRegistry);
    ecsWorld->SetPoolFactory(std::make_unique<mem_alloc_fixed_block::FixedBlockPoolFactory>());
    ecsWorld->SetAllocatorFactory(std::make_unique<mem_alloc_fixed_block::FixedBlockAllocatorFactory>());
    ecsWorld->CreateWorld();

    // Create entity
    riaecs::Entity entity = ecsWorld->CreateEntity();
    ecsWorld->AddComponent(entity, mono_transform::ComponentTransformID());
    {
        mono_transform::ComponentTransform* transform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, entity, mono_transform::ComponentTransformID());

        mono_transform::ComponentTransform::SetupParam param;
        param.pos_ = DirectX::XMFLOAT3(10.0f, 0.0f, 0.0f);
        transform->Setup(param);

        // Set world scale
        transform->SetScale(XMFLOAT3(2.0f, 2.0f, 2.0f), *ecsWorld);
        EXPECT_FLOAT_EQ(transform->GetScale().x, 2.0f);

        // Set local scale (no parent, so world scale should be same as local)
        transform->SetLocalScale(XMFLOAT3(3.0f, 3.0f, 3.0f), *ecsWorld);
        EXPECT_FLOAT_EQ(transform->GetScale().x, 2.0f);
        EXPECT_FLOAT_EQ(transform->GetLocalScale().x, 3.0f);
    }
}

TEST(Transform, ParentChildRelationShip)
{
    std::unique_ptr<riaecs::IECSWorld> ecsWorld = std::make_unique<riaecs::ECSWorld>(*riaecs::gComponentFactoryRegistry, *riaecs::gComponentMaxCountRegistry);
    ecsWorld->SetPoolFactory(std::make_unique<mem_alloc_fixed_block::FixedBlockPoolFactory>());
    ecsWorld->SetAllocatorFactory(std::make_unique<mem_alloc_fixed_block::FixedBlockAllocatorFactory>());
    ecsWorld->CreateWorld();

    // Create parent entity
    riaecs::Entity parentEntity = ecsWorld->CreateEntity();
    ecsWorld->AddComponent(parentEntity, mono_transform::ComponentTransformID());
    {
        mono_transform::ComponentTransform* parentTransform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, parentEntity, mono_transform::ComponentTransformID());

        mono_transform::ComponentTransform::SetupParam param;
        param.pos_ = DirectX::XMFLOAT3(10.0f, 0.0f, 0.0f);
        parentTransform->Setup(param);
    }

    // Create child entity
    riaecs::Entity childEntity = ecsWorld->CreateEntity();
    ecsWorld->AddComponent(childEntity, mono_transform::ComponentTransformID());
    {
        mono_transform::ComponentTransform* childTransform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, childEntity, mono_transform::ComponentTransformID());

        mono_transform::ComponentTransform::SetupParam param;
        param.pos_ = DirectX::XMFLOAT3(5.0f, 0.0f, 0.0f);
        childTransform->Setup(param);

        // Set parent-child relationship
        childTransform->SetParent(childEntity, parentEntity, *ecsWorld);
    }

    // Check if the child's parent is set correctly
    {
        mono_transform::ComponentTransform* childTransform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, childEntity, mono_transform::ComponentTransformID());

        EXPECT_EQ(childTransform->GetParent(), parentEntity);
    }

    // Check if the parent has the child in its child list
    {
        mono_transform::ComponentTransform* parentTransform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, parentEntity, mono_transform::ComponentTransformID());

        auto it = std::find(parentTransform->GetChilds().begin(), parentTransform->GetChilds().end(), childEntity);
        EXPECT_TRUE(it != parentTransform->GetChilds().end());
    }

    // Update transforms
    {
        mono_transform::ComponentTransform* parentTransform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, parentEntity, mono_transform::ComponentTransformID());

        mono_transform::UpdateRootTransform(parentTransform);

        EXPECT_FLOAT_EQ(parentTransform->GetPos().x, 10.0f);
        EXPECT_FLOAT_EQ(parentTransform->GetPos().y, 0.0f);
        EXPECT_FLOAT_EQ(parentTransform->GetPos().z, 0.0f);
    }

    {
        mono_transform::ComponentTransform* childTransform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, childEntity, mono_transform::ComponentTransformID());

        mono_transform::ComponentTransform* parentTransform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, parentEntity, mono_transform::ComponentTransformID());

        mono_transform::UpdateChildTransform(childTransform, parentTransform->GetWorldMatrix(), *ecsWorld);

        EXPECT_FLOAT_EQ(childTransform->GetPos().x, 5.0f); // World position remains unchanged
        EXPECT_FLOAT_EQ(childTransform->GetPos().y, 0.0f);
        EXPECT_FLOAT_EQ(childTransform->GetPos().z, 0.0f);

        EXPECT_FLOAT_EQ(childTransform->GetLocalPos().x, -5.0f); // Local position updated based on parent
        EXPECT_FLOAT_EQ(childTransform->GetLocalPos().y, 0.0f);
        EXPECT_FLOAT_EQ(childTransform->GetLocalPos().z, 0.0f);
    }

    // Edit child local position
    {
        mono_transform::ComponentTransform* childTransform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, childEntity, mono_transform::ComponentTransformID());

        childTransform->SetLocalPos(XMFLOAT3(0.0f, 5.0f, 0.0f), *ecsWorld);
    }

    // Update transforms again
    {
        mono_transform::ComponentTransform* parentTransform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, parentEntity, mono_transform::ComponentTransformID());

        mono_transform::UpdateRootTransform(parentTransform);

        EXPECT_FLOAT_EQ(parentTransform->GetPos().x, 10.0f);
        EXPECT_FLOAT_EQ(parentTransform->GetPos().y, 0.0f);
        EXPECT_FLOAT_EQ(parentTransform->GetPos().z, 0.0f);
    }

    {
        mono_transform::ComponentTransform* childTransform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, childEntity, mono_transform::ComponentTransformID());

        mono_transform::ComponentTransform* parentTransform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, parentEntity, mono_transform::ComponentTransformID());

        mono_transform::UpdateChildTransform(childTransform, parentTransform->GetWorldMatrix(), *ecsWorld);

        // World position updated based on new local position
        EXPECT_FLOAT_EQ(childTransform->GetPos().x, 10.0f); // 10 (parent) + 0 (local)
        EXPECT_FLOAT_EQ(childTransform->GetPos().y, 5.0f); // 0 (parent) + 5 (local)
        EXPECT_FLOAT_EQ(childTransform->GetPos().z, 0.0f);
    }

    // Remove child from parent
    {
        mono_transform::ComponentTransform* parentTransform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, parentEntity, mono_transform::ComponentTransformID());

        parentTransform->RemoveChild(parentEntity, childEntity, *ecsWorld);
    }

    // Check if the child's parent is reset
    {
        mono_transform::ComponentTransform* childTransform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, childEntity, mono_transform::ComponentTransformID());

        EXPECT_EQ(childTransform->GetParent().IsValid(), false);
    }

    // Check if the parent no longer has the child in its child list
    {
        mono_transform::ComponentTransform* parentTransform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, parentEntity, mono_transform::ComponentTransformID());

        auto it = std::find(parentTransform->GetChilds().begin(), parentTransform->GetChilds().end(), childEntity);
        EXPECT_TRUE(it == parentTransform->GetChilds().end());
    }

    // Update transforms again
    {
        mono_transform::ComponentTransform* parentTransform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, parentEntity, mono_transform::ComponentTransformID());

        mono_transform::UpdateRootTransform(parentTransform);

        EXPECT_FLOAT_EQ(parentTransform->GetPos().x, 10.0f);
        EXPECT_FLOAT_EQ(parentTransform->GetPos().y, 0.0f);
        EXPECT_FLOAT_EQ(parentTransform->GetPos().z, 0.0f);
    }

    {
        mono_transform::ComponentTransform* childTransform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, childEntity, mono_transform::ComponentTransformID());

        // Child transform remains unchanged since it has no parent now
        EXPECT_FLOAT_EQ(childTransform->GetPos().x, 10.0f);
        EXPECT_FLOAT_EQ(childTransform->GetPos().y, 5.0f);
        EXPECT_FLOAT_EQ(childTransform->GetPos().z, 0.0f);

        // Local position reset to zero since it has no parent
        EXPECT_FLOAT_EQ(childTransform->GetLocalPos().x, 0.0f);
        EXPECT_FLOAT_EQ(childTransform->GetLocalPos().y, 0.0f);
        EXPECT_FLOAT_EQ(childTransform->GetLocalPos().z, 0.0f);
    }

}

TEST(Transform, RecursivelyUpdateChilds)
{
    std::unique_ptr<riaecs::IECSWorld> ecsWorld = std::make_unique<riaecs::ECSWorld>(*riaecs::gComponentFactoryRegistry, *riaecs::gComponentMaxCountRegistry);
    ecsWorld->SetPoolFactory(std::make_unique<mem_alloc_fixed_block::FixedBlockPoolFactory>());
    ecsWorld->SetAllocatorFactory(std::make_unique<mem_alloc_fixed_block::FixedBlockAllocatorFactory>());
    ecsWorld->CreateWorld();

    // Create parent entity
    riaecs::Entity parentEntity = ecsWorld->CreateEntity();
    ecsWorld->AddComponent(parentEntity, mono_transform::ComponentTransformID());
    {
        mono_transform::ComponentTransform* parentTransform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, parentEntity, mono_transform::ComponentTransformID());

        mono_transform::ComponentTransform::SetupParam param;
        param.pos_ = DirectX::XMFLOAT3(10.0f, 0.0f, 0.0f);
        parentTransform->Setup(param);
    }

    // Create child entity
    riaecs::Entity childEntity = ecsWorld->CreateEntity();
    ecsWorld->AddComponent(childEntity, mono_transform::ComponentTransformID());
    {
        mono_transform::ComponentTransform* childTransform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, childEntity, mono_transform::ComponentTransformID());

        mono_transform::ComponentTransform::SetupParam param;
        param.pos_ = DirectX::XMFLOAT3(5.0f, 0.0f, 0.0f);
        childTransform->Setup(param);

        // Set parent-child relationship
        childTransform->SetParent(childEntity, parentEntity, *ecsWorld);
    }

    // Create grandchild entity
    riaecs::Entity grandChildEntity = ecsWorld->CreateEntity();
    ecsWorld->AddComponent(grandChildEntity, mono_transform::ComponentTransformID());
    {
        mono_transform::ComponentTransform* grandChildTransform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, grandChildEntity, mono_transform::ComponentTransformID());

        mono_transform::ComponentTransform::SetupParam param;
        param.pos_ = DirectX::XMFLOAT3(2.0f, 0.0f, 0.0f);
        grandChildTransform->Setup(param);

        // Set child-grandchild relationship
        grandChildTransform->SetParent(grandChildEntity, childEntity, *ecsWorld);
    }

    // Create other child entity
    riaecs::Entity otherChildEntity = ecsWorld->CreateEntity();
    ecsWorld->AddComponent(otherChildEntity, mono_transform::ComponentTransformID());
    {
        mono_transform::ComponentTransform* otherChildTransform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, otherChildEntity, mono_transform::ComponentTransformID());

        mono_transform::ComponentTransform::SetupParam param;
        param.pos_ = DirectX::XMFLOAT3(8.0f, 0.0f, 0.0f);
        otherChildTransform->Setup(param);

        // Set parent-otherChild relationship
        otherChildTransform->SetParent(otherChildEntity, parentEntity, *ecsWorld);
    }

    // Move parent
    {
        mono_transform::ComponentTransform* parentTransform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, parentEntity, mono_transform::ComponentTransformID());

        parentTransform->SetPos(XMFLOAT3(20.0f, 50.0f, 0.0f), *ecsWorld);
    }

    // Update transforms
    {
        mono_transform::ComponentTransform* parentTransform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, parentEntity, mono_transform::ComponentTransformID());

        EXPECT_FALSE(parentTransform->GetParent().IsValid());

        mono_transform::UpdateRootTransform(parentTransform);
        mono_transform::UpdateChildsTransform(parentEntity, *ecsWorld);
    }

    // Check positions
    {
        mono_transform::ComponentTransform* parentTransform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, parentEntity, mono_transform::ComponentTransformID());
        EXPECT_FLOAT_EQ(parentTransform->GetPos().x, 20.0f);
        EXPECT_FLOAT_EQ(parentTransform->GetPos().y, 50.0f);
        EXPECT_FLOAT_EQ(parentTransform->GetPos().z, 0.0f);
    }
    {
        mono_transform::ComponentTransform* childTransform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, childEntity, mono_transform::ComponentTransformID());
        EXPECT_FLOAT_EQ(childTransform->GetPos().x, 15.0f); // 20 (parent) + -5 (local)
        EXPECT_FLOAT_EQ(childTransform->GetPos().y, 50.0f);
        EXPECT_FLOAT_EQ(childTransform->GetPos().z, 0.0f);
    }
    {
        mono_transform::ComponentTransform* grandChildTransform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, grandChildEntity, mono_transform::ComponentTransformID());
        EXPECT_FLOAT_EQ(grandChildTransform->GetPos().x, 12.0f); // 20 (parent) + -5 (child local) + -3 (grandchild local)
        EXPECT_FLOAT_EQ(grandChildTransform->GetPos().y, 50.0f);
        EXPECT_FLOAT_EQ(grandChildTransform->GetPos().z, 0.0f);
    }
    {
        mono_transform::ComponentTransform* otherChildTransform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, otherChildEntity, mono_transform::ComponentTransformID());
        EXPECT_FLOAT_EQ(otherChildTransform->GetPos().x, 18.0f); // 20 (parent) + -2 (local)
        EXPECT_FLOAT_EQ(otherChildTransform->GetPos().y, 50.0f);
        EXPECT_FLOAT_EQ(otherChildTransform->GetPos().z, 0.0f);
    }
}

TEST(Transform, InitializeWithAnchor)
{
    mono_transform::ComponentTransform transform;

    mono_transform::Anchor anchor;
    anchor.size_ = XMFLOAT2(1920.0f, 1080.0f);
    anchor.pivot_ = XMFLOAT2(0.5f, 0.5f); // center

    mono_transform::ComponentTransform::SetupParamWithAnchor param;
    param.anchor_ = anchor;
    param.anchoredPos_ = XMFLOAT2(0.0f, 0.0f); // centered
    param.depth_ = 0.0f;
    param.anchoredSize_ = XMFLOAT2(100.0f, 100.0f);
    transform.Setup(param);

    EXPECT_EQ(transform.IsInitialized(), true);
    EXPECT_FLOAT_EQ(transform.GetPos().x, 0.0f);
    EXPECT_FLOAT_EQ(transform.GetPos().y, 0.0f);
    EXPECT_FLOAT_EQ(transform.GetPos().z, 0.0f);
    EXPECT_FLOAT_EQ(transform.GetScale().x, 100.0f / 1920.0f);
    EXPECT_FLOAT_EQ(transform.GetScale().y, 100.0f / 1080.0f);
    EXPECT_FLOAT_EQ(transform.GetScale().z, 1.0f);
}

TEST(Transform, AnchoredPosition)
{
    std::unique_ptr<riaecs::IECSWorld> ecsWorld = std::make_unique<riaecs::ECSWorld>(*riaecs::gComponentFactoryRegistry, *riaecs::gComponentMaxCountRegistry);
    ecsWorld->SetPoolFactory(std::make_unique<mem_alloc_fixed_block::FixedBlockPoolFactory>());
    ecsWorld->SetAllocatorFactory(std::make_unique<mem_alloc_fixed_block::FixedBlockAllocatorFactory>());
    ecsWorld->CreateWorld();

    // Create entity
    riaecs::Entity entity = ecsWorld->CreateEntity();
    ecsWorld->AddComponent(entity, mono_transform::ComponentTransformID());
    {
        mono_transform::ComponentTransform* transform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, entity, mono_transform::ComponentTransformID());

        mono_transform::Anchor anchor;
        anchor.size_ = XMFLOAT2(1920.0f, 1080.0f);
        anchor.pivot_ = XMFLOAT2(0.5f, 0.5f); // center

        mono_transform::ComponentTransform::SetupParamWithAnchor param;
        param.anchor_ = anchor;
        param.anchoredPos_ = XMFLOAT2(0.0f, 0.0f); // centered
        param.depth_ = 0.0f;
        param.anchoredSize_ = XMFLOAT2(100.0f, 100.0f);
        transform->Setup(param);

        // Set anchored world position to top-left corner
        transform->SetPosWithAnchor(anchor, XMFLOAT2(-960.0f, 540.0f), 0.0f, *ecsWorld);
        EXPECT_FLOAT_EQ(transform->GetPos().x, -1.0f);
        EXPECT_FLOAT_EQ(transform->GetPos().y, 1.0f);
        EXPECT_FLOAT_EQ(transform->GetPos().z, 0.0f);

        // Set anchored local position
        transform->SetLocalPosWithAnchor(anchor, XMFLOAT2(10.0f, 10.0f), 0.0f, *ecsWorld);
        EXPECT_NEAR(transform->GetLocalPos().x, 10.0f / (1920.0f / 2.0f), 1e-6f);
        EXPECT_NEAR(transform->GetLocalPos().y, 10.0f / (1080.0f / 2.0f), 1e-6f);
        EXPECT_FLOAT_EQ(transform->GetLocalPos().z, 0.0f);
    }
}

TEST(Transform, AnchoredSize)
{
    std::unique_ptr<riaecs::IECSWorld> ecsWorld = std::make_unique<riaecs::ECSWorld>(*riaecs::gComponentFactoryRegistry, *riaecs::gComponentMaxCountRegistry);
    ecsWorld->SetPoolFactory(std::make_unique<mem_alloc_fixed_block::FixedBlockPoolFactory>());
    ecsWorld->SetAllocatorFactory(std::make_unique<mem_alloc_fixed_block::FixedBlockAllocatorFactory>());
    ecsWorld->CreateWorld();

    // Create entity
    riaecs::Entity entity = ecsWorld->CreateEntity();
    ecsWorld->AddComponent(entity, mono_transform::ComponentTransformID());
    {
        mono_transform::ComponentTransform* transform
        = riaecs::GetComponent<mono_transform::ComponentTransform>(*ecsWorld, entity, mono_transform::ComponentTransformID());

        mono_transform::Anchor anchor;
        anchor.size_ = XMFLOAT2(1920.0f, 1080.0f);
        anchor.pivot_ = XMFLOAT2(0.5f, 0.5f); // center

        mono_transform::ComponentTransform::SetupParamWithAnchor param;
        param.anchor_ = anchor;
        param.anchoredPos_ = XMFLOAT2(0.0f, 0.0f); // centered
        param.depth_ = 0.0f;
        param.anchoredSize_ = XMFLOAT2(100.0f, 100.0f);
        transform->Setup(param);

        // Set anchored size to 200x100
        transform->SetScaleWithAnchor(anchor, XMFLOAT2(200.0f, 200.0f), *ecsWorld);
        EXPECT_NEAR(transform->GetScale().x, 200.0f / 1920.0f, 1e-6f);
        EXPECT_NEAR(transform->GetScale().y, 200.0f / 1080.0f, 1e-6f);
        EXPECT_FLOAT_EQ(transform->GetScale().z, 1.0f);

        // Set local anchored size to 300x150
        transform->SetLocalScaleWithAnchor(anchor, XMFLOAT2(300.0f, 150.0f), *ecsWorld);
        EXPECT_NEAR(transform->GetLocalScale().x, 300.0f / 1920.0f, 1e-6f);
        EXPECT_NEAR(transform->GetLocalScale().y, 150.0f / 1080.0f, 1e-6f);
        EXPECT_FLOAT_EQ(transform->GetLocalScale().z, 1.0f);
    }
}