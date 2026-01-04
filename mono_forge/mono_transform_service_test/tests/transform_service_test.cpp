#include "mono_transform_service_test/pch.h"

using namespace DirectX;

#include "mono_service/include/service_importer.h"
#include "mono_service/include/thread_affinity.h"

#include "mono_transform_service/include/transform_service.h"
#include "mono_transform_service/include/transform_service_command_list.h"
#include "mono_transform_service/include/transform_service_view.h"

namespace mono_transform_service_test
{

bool ImportTransformService(mono_service::ServiceRegistry& service_registry)
{
    // Create service importer
    std::unique_ptr<mono_service::ServiceImporter> service_importer 
        = std::make_unique<mono_service::ServiceImporter>(service_registry);

    // Import transform service in to registry
    std::unique_ptr<mono_transform_service::TransformService> transform_service 
        = std::make_unique<mono_transform_service::TransformService>(0);

    // Create setup parameters
    mono_transform_service::TransformService::SetupParam setup_param;

    // Import transform service
    return service_importer->Import(
        std::move(transform_service), mono_transform_service::TransformServiceHandle::ID(), setup_param);
}

} // namespace mono_transform_service_test

TEST(TransformService, Import)
{
    /*******************************************************************************************************************
     * Import transform service
    /******************************************************************************************************************/

    // Create service id generator
    std::unique_ptr<mono_service::ServiceIDGenerator> service_id_generator 
        = std::make_unique<mono_service::ServiceIDGenerator>();

    // Create service registry
    std::unique_ptr<mono_service::ServiceRegistry> service_registry 
        = std::make_unique<mono_service::ServiceRegistry>();

    // Import transform service in to registry
    bool result = mono_transform_service_test::ImportTransformService(*service_registry);
    ASSERT_TRUE(result);
}

TEST(TransformService, Execute)
{
    /*******************************************************************************************************************
     * Import transform service
    /******************************************************************************************************************/

    // Create service id generator
    std::unique_ptr<mono_service::ServiceIDGenerator> service_id_generator 
        = std::make_unique<mono_service::ServiceIDGenerator>();

    // Create service registry
    std::unique_ptr<mono_service::ServiceRegistry> service_registry 
        = std::make_unique<mono_service::ServiceRegistry>();

    // Import transform service in to registry
    bool result = mono_transform_service_test::ImportTransformService(*service_registry);
    ASSERT_TRUE(result);

    // Get transform service proxy from transform service
    std::unique_ptr<mono_service::ServiceProxy> transform_service_proxy = nullptr;
    service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        // Get transform service
        mono_service::Service* service 
            = &registry.Get(mono_transform_service::TransformServiceHandle::ID());
        mono_transform_service::TransformService& transform_service
            = dynamic_cast<mono_transform_service::TransformService&>(*service);

        // Create service proxy
        transform_service_proxy = transform_service.CreateServiceProxy();
    });

    /*******************************************************************************************************************
     * Create transforms
    /******************************************************************************************************************/

    // Transform handle
    transform_evaluator::TransformHandle transform_handle;
    XMFLOAT3 translation = XMFLOAT3(1.0f, 2.0f, 3.0f);
    XMFLOAT4 rotation = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
    XMFLOAT3 scale = XMFLOAT3(2.0f, 2.0f, 2.0f);

    mono_service::ServiceProgress progress = 0;
    {
        // Create command list
        std::unique_ptr<mono_service::ServiceCommandList> command_list 
            = transform_service_proxy->CreateCommandList();
        mono_transform_service::TransformServiceCommandList& transform_command_list
            = dynamic_cast<mono_transform_service::TransformServiceCommandList&>(*command_list);

        // Create transform in transform service
        transform_command_list.CreateTransform(transform_handle, translation, rotation, scale);

        // Submit command list to transform service
        progress = transform_service_proxy->SubmitCommandList(std::move(command_list));
    }

    service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        // Get graphics service
        mono_service::Service& service = registry.Get(mono_transform_service::TransformServiceHandle::ID());

        // Pre-update
        result = service.PreUpdate();
        ASSERT_TRUE(result);

        // Update
        result = service.Update();
        ASSERT_TRUE(result);

        // Post-update
        result = service.PostUpdate();
        ASSERT_TRUE(result);

        // Check progress
        ASSERT_EQ(service.GetProgress(), progress + 1);
    });

    /*******************************************************************************************************************
     * Get and verify transform
    /******************************************************************************************************************/

    {
        // Create view
        std::unique_ptr<mono_service::ServiceView> service_view
            = transform_service_proxy->CreateView();
        mono_transform_service::TransformServiceView& transform_view
            = dynamic_cast<mono_transform_service::TransformServiceView&>(*service_view);

        // Get world matrix
        XMMATRIX world_matrix = transform_view.GetWorldMatrix(transform_handle);

        // Get translation
        const XMFLOAT3& got_translation = transform_view.GetTranslation(transform_handle);
        ASSERT_FLOAT_EQ(got_translation.x, translation.x);
        ASSERT_FLOAT_EQ(got_translation.y, translation.y);
        ASSERT_FLOAT_EQ(got_translation.z, translation.z);

        // Get rotation
        const XMFLOAT4& got_rotation = transform_view.GetRotation(transform_handle);
        ASSERT_FLOAT_EQ(got_rotation.x, rotation.x);
        ASSERT_FLOAT_EQ(got_rotation.y, rotation.y);
        ASSERT_FLOAT_EQ(got_rotation.z, rotation.z);
        ASSERT_FLOAT_EQ(got_rotation.w, rotation.w);

        // Get scale
        const XMFLOAT3& got_scale = transform_view.GetScale(transform_handle);
        ASSERT_FLOAT_EQ(got_scale.x, scale.x);
        ASSERT_FLOAT_EQ(got_scale.y, scale.y);
        ASSERT_FLOAT_EQ(got_scale.z, scale.z);
    }

    /*******************************************************************************************************************
     * Update transform
    /******************************************************************************************************************/

    translation = XMFLOAT3(4.0f, 5.0f, 6.0f);
    rotation = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
    scale = XMFLOAT3(3.0f, 3.0f, 3.0f);
    {
        // Create command list
        std::unique_ptr<mono_service::ServiceCommandList> command_list 
            = transform_service_proxy->CreateCommandList();
        mono_transform_service::TransformServiceCommandList& transform_command_list
            = dynamic_cast<mono_transform_service::TransformServiceCommandList&>(*command_list);

        // Update transform in transform service
        transform_command_list.UpdateWorldTranslation(transform_handle, translation);
        transform_command_list.UpdateWorldRotation(transform_handle, rotation);
        transform_command_list.UpdateWorldScale(transform_handle, scale);

        // Submit command list to transform service
        progress = transform_service_proxy->SubmitCommandList(std::move(command_list));
    }

    service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        // Get graphics service
        mono_service::Service& service = registry.Get(mono_transform_service::TransformServiceHandle::ID());

        // Pre-update
        result = service.PreUpdate();
        ASSERT_TRUE(result);

        // Update
        result = service.Update();
        ASSERT_TRUE(result);

        // Post-update
        result = service.PostUpdate();
        ASSERT_TRUE(result);

        // Check progress
        ASSERT_EQ(service.GetProgress(), progress + 1);
    });

    /*******************************************************************************************************************
     * Get and verify updated transform
    /******************************************************************************************************************/

    {
        // Create view
        std::unique_ptr<mono_service::ServiceView> service_view
            = transform_service_proxy->CreateView();
        mono_transform_service::TransformServiceView& transform_view
            = dynamic_cast<mono_transform_service::TransformServiceView&>(*service_view);

        // Get world matrix
        XMMATRIX world_matrix = transform_view.GetWorldMatrix(transform_handle);

        // Get translation
        const XMFLOAT3& got_translation = transform_view.GetTranslation(transform_handle);
        ASSERT_FLOAT_EQ(got_translation.x, translation.x);
        ASSERT_FLOAT_EQ(got_translation.y, translation.y);
        ASSERT_FLOAT_EQ(got_translation.z, translation.z);

        // Get rotation
        const XMFLOAT4& got_rotation = transform_view.GetRotation(transform_handle);
        ASSERT_FLOAT_EQ(got_rotation.x, rotation.x);
        ASSERT_FLOAT_EQ(got_rotation.y, rotation.y);
        ASSERT_FLOAT_EQ(got_rotation.z, rotation.z);
        ASSERT_FLOAT_EQ(got_rotation.w, rotation.w);

        // Get scale
        const XMFLOAT3& got_scale = transform_view.GetScale(transform_handle);
        ASSERT_FLOAT_EQ(got_scale.x, scale.x);
        ASSERT_FLOAT_EQ(got_scale.y, scale.y);
        ASSERT_FLOAT_EQ(got_scale.z, scale.z);
    }

    /*******************************************************************************************************************
     * Destroy transform
    /******************************************************************************************************************/

    {
        // Create command list
        std::unique_ptr<mono_service::ServiceCommandList> command_list 
            = transform_service_proxy->CreateCommandList();
        mono_transform_service::TransformServiceCommandList& transform_command_list
            = dynamic_cast<mono_transform_service::TransformServiceCommandList&>(*command_list);

        // Erase transform in transform service
        transform_command_list.DestroyTransform(transform_handle);

        // Submit command list to transform service
        progress = transform_service_proxy->SubmitCommandList(std::move(command_list));
    }

    service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        // Get graphics service
        mono_service::Service& service = registry.Get(mono_transform_service::TransformServiceHandle::ID());

        // Pre-update
        result = service.PreUpdate();
        ASSERT_TRUE(result);

        // Update
        result = service.Update();
        ASSERT_TRUE(result);

        // Post-update
        result = service.PostUpdate();
        ASSERT_TRUE(result);

        // Check progress
        ASSERT_EQ(service.GetProgress(), progress + 1);
    });

    /*******************************************************************************************************************
     * Verify transform destruction
    /******************************************************************************************************************/

    {
        // Create view
        std::unique_ptr<mono_service::ServiceView> service_view
            = transform_service_proxy->CreateView();
        mono_transform_service::TransformServiceView& transform_view
            = dynamic_cast<mono_transform_service::TransformServiceView&>(*service_view);

        // Verify that accessing the destroyed transform results in an assertion failure
        ASSERT_DEATH({transform_view.GetWorldMatrix(transform_handle);}, ".*");
    }

    /*******************************************************************************************************************
     * Cleanup
    /******************************************************************************************************************/

    // Cleanup services
    service_registry.reset();
}