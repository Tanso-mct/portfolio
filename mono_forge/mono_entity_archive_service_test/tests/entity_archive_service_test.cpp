#include "mono_entity_archive_service_test/pch.h"

#include "mono_service/include/service_importer.h"
#include "mono_service/include/thread_affinity.h"
#include "mono_service/include/service_proxy_manager.h"

#include "mono_entity_archive_service/include/entity_archive_service.h"
#include "mono_entity_archive_service/include/entity_archive_service_command_list.h"
#include "mono_entity_archive_service/include/entity_archive_service_view.h"

#include "mono_entity_archive_service_test/tests/component_reflection.h"
#include "mono_entity_archive_service_test/tests/test_meta_component.h"
#include "mono_entity_archive_service_test/tests/test_transform_component.h"

TEST(EntityArchiveServiceView, Import)
{
    bool result = false;

    // Create component id generator
    std::unique_ptr<ecs::ComponentIDGenerator> component_id_generator
        = std::make_unique<ecs::ComponentIDGenerator>();

    /*******************************************************************************************************************
     * Import services
    /******************************************************************************************************************/

    // Create service id generator
    std::unique_ptr<mono_service::ServiceIDGenerator> service_id_generator
        = std::make_unique<mono_service::ServiceIDGenerator>();

    // Create service registry
    std::unique_ptr<mono_service::ServiceRegistry> service_registry
        = std::make_unique<mono_service::ServiceRegistry>();

    // Import entity archive service in to registry
    constexpr mono_service::ServiceThreadAffinityID ENTITY_ARCHIVE_SERVICE_THREAD_AFFINITY_ID = 0;
    {
        mono_entity_archive_service::EntityArchiveService::SetupParam entity_archive_service_setup_param;

        // Register component IDs and names
        component_editor::ComponentNameMap& component_name_map = entity_archive_service_setup_param.component_name_map;
        component_name_map[component_editor_test::TestMetaComponentHandle::ID()] 
            = "component_editor_test::TestMetaComponent";
        component_name_map[component_editor_test::TestTransformComponentHandle::ID()] 
            = "component_editor_test::TestTransformComponent";

        // Register component adders
        component_editor::ComponentAdderMap& component_adder_map = entity_archive_service_setup_param.component_adder_map;
        component_adder_map[component_editor_test::TestMetaComponentHandle::ID()] 
            = std::make_unique<component_editor_test::TestMetaComponentAdder>();
        component_adder_map[component_editor_test::TestTransformComponentHandle::ID()] 
            = std::make_unique<component_editor_test::TestTransformComponentAdder>();

        // Copy component reflection registry
        entity_archive_service_setup_param.component_reflection_registry 
            = component_editor_test::g_component_reflection_registry;

        // Copy component field value setter
        entity_archive_service_setup_param.setup_param_field_value_setter
            = component_editor_test::g_setup_param_field_value_setter;

        result = mono_service::ImportService<
            mono_entity_archive_service::EntityArchiveService,
            mono_entity_archive_service::EntityArchiveServiceHandle>(
                *service_registry, ENTITY_ARCHIVE_SERVICE_THREAD_AFFINITY_ID, 
                entity_archive_service_setup_param);
        ASSERT_TRUE(result);
    }

    // Create entity archive service proxy
    std::unique_ptr<mono_service::ServiceProxy> entity_archive_service_proxy = nullptr;
    service_registry->WithUniqueLock([&](const mono_service::ServiceRegistry& registry)
    {
        // Get the service
        mono_service::Service& service = registry.Get(
            mono_entity_archive_service::EntityArchiveServiceHandle::ID());

        // Create the service proxy
        entity_archive_service_proxy = service.CreateServiceProxy();
    });
    ASSERT_NE(entity_archive_service_proxy, nullptr);
}

TEST(EntityArchiveServiceView, Edit)
{
    bool result = false;

    // Create component id generator
    std::unique_ptr<ecs::ComponentIDGenerator> component_id_generator
        = std::make_unique<ecs::ComponentIDGenerator>();

    /*******************************************************************************************************************
     * Import services
    /******************************************************************************************************************/

    // Create service id generator
    std::unique_ptr<mono_service::ServiceIDGenerator> service_id_generator
        = std::make_unique<mono_service::ServiceIDGenerator>();

    // Create service registry
    std::unique_ptr<mono_service::ServiceRegistry> service_registry
        = std::make_unique<mono_service::ServiceRegistry>();

    // Import entity archive service in to registry
    constexpr mono_service::ServiceThreadAffinityID ENTITY_ARCHIVE_SERVICE_THREAD_AFFINITY_ID = 0;
    {
        mono_entity_archive_service::EntityArchiveService::SetupParam entity_archive_service_setup_param;

        // Register component IDs and names
        component_editor::ComponentNameMap& component_name_map = entity_archive_service_setup_param.component_name_map;
        component_name_map[component_editor_test::TestMetaComponentHandle::ID()] 
            = "component_editor_test::TestMetaComponent";
        component_name_map[component_editor_test::TestTransformComponentHandle::ID()] 
            = "component_editor_test::TestTransformComponent";

        // Register component adders
        component_editor::ComponentAdderMap& component_adder_map = entity_archive_service_setup_param.component_adder_map;
        component_adder_map[component_editor_test::TestMetaComponentHandle::ID()] 
            = std::make_unique<component_editor_test::TestMetaComponentAdder>();
        component_adder_map[component_editor_test::TestTransformComponentHandle::ID()] 
            = std::make_unique<component_editor_test::TestTransformComponentAdder>();

        // Copy component reflection registry
        entity_archive_service_setup_param.component_reflection_registry 
            = component_editor_test::g_component_reflection_registry;

        // Copy component field value setter
        entity_archive_service_setup_param.setup_param_field_value_setter
            = component_editor_test::g_setup_param_field_value_setter;

        result = mono_service::ImportService<
            mono_entity_archive_service::EntityArchiveService,
            mono_entity_archive_service::EntityArchiveServiceHandle>(
                *service_registry, ENTITY_ARCHIVE_SERVICE_THREAD_AFFINITY_ID, 
                entity_archive_service_setup_param);
        ASSERT_TRUE(result);
    }

    // Create entity archive service proxy
    std::unique_ptr<mono_service::ServiceProxy> entity_archive_service_proxy = nullptr;
    service_registry->WithUniqueLock([&](const mono_service::ServiceRegistry& registry)
    {
        // Get the service
        mono_service::Service& service = registry.Get(
            mono_entity_archive_service::EntityArchiveServiceHandle::ID());

        // Create the service proxy
        entity_archive_service_proxy = service.CreateServiceProxy();
    });
    ASSERT_NE(entity_archive_service_proxy, nullptr);

    /*******************************************************************************************************************
     * Add setup params
    /******************************************************************************************************************/

    // Create test entity
    ecs::Entity test_entity(0, 0);

    {
        // Create test meta component setup param
        std::unique_ptr<component_editor_test::TestMetaComponent::SetupParam> meta_setup_param 
            = std::make_unique<component_editor_test::TestMetaComponent::SetupParam>();

        // Create test transform component setup param
        std::unique_ptr<component_editor_test::TestTransformComponent::SetupParam> transform_setup_param 
            = std::make_unique<component_editor_test::TestTransformComponent::SetupParam>();

        // Create entity archive service command list
        std::unique_ptr<mono_service::ServiceCommandList> command_list 
            = entity_archive_service_proxy->CreateCommandList();
        mono_entity_archive_service::EntityArchiveServiceCommandList* entity_archive_command_list 
            = dynamic_cast<mono_entity_archive_service::EntityArchiveServiceCommandList*>(command_list.get());
        ASSERT_NE(entity_archive_command_list, nullptr);

        // Add setup param to entity archive service
        entity_archive_command_list->AddSetupParam(
            test_entity, component_editor_test::TestMetaComponentHandle::ID(), std::move(meta_setup_param));
        entity_archive_command_list->AddSetupParam(
            test_entity, component_editor_test::TestTransformComponentHandle::ID(), std::move(transform_setup_param));

        // Submit command list
        entity_archive_service_proxy->SubmitCommandList(std::move(command_list));
    }

    /*******************************************************************************************************************
     * Update service
    /******************************************************************************************************************/

    service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        // Get the service
        mono_service::Service& service = registry.Get(
            mono_entity_archive_service::EntityArchiveServiceHandle::ID());

        // Pre-update
        result = service.PreUpdate();
        ASSERT_TRUE(result);

        // Update
        result = service.Update();
        ASSERT_TRUE(result);

        // Post-update
        result = service.PostUpdate();
        ASSERT_TRUE(result);
    });

    /*******************************************************************************************************************
     * Verify setup params
    /******************************************************************************************************************/

    {
        // Create entity archive service view
        std::unique_ptr<mono_service::ServiceView> service_view 
            = entity_archive_service_proxy->CreateView();
        mono_entity_archive_service::EntityArchiveServiceView* entity_archive_service_view 
            = dynamic_cast<mono_entity_archive_service::EntityArchiveServiceView*>(service_view.get());
        ASSERT_NE(entity_archive_service_view, nullptr);

        // Get and verify test meta component setup param
        {
            utility_header::ConstSharedLockedValue<ecs::Component::SetupParam> setup_param = 
                entity_archive_service_view->GetSetupParam(
                    test_entity, component_editor_test::TestMetaComponentHandle::ID());

            const component_editor_test::TestMetaComponent::SetupParam* meta_setup_param = 
                dynamic_cast<const component_editor_test::TestMetaComponent::SetupParam*>(&setup_param());
            ASSERT_NE(meta_setup_param, nullptr);
            EXPECT_EQ(meta_setup_param->name, component_editor_test::DEFAULT_NAME);
            EXPECT_EQ(meta_setup_param->active_self, component_editor_test::DEFAULT_ACTIVE_SELF);
            EXPECT_EQ(meta_setup_param->tag, component_editor_test::DEFAULT_TAG);
            EXPECT_EQ(meta_setup_param->layer, component_editor_test::DEFAULT_LAYER);
        }

        // Get and verify test transform component setup param
        {
            utility_header::ConstSharedLockedValue<ecs::Component::SetupParam> setup_param = 
                entity_archive_service_view->GetSetupParam(
                    test_entity, component_editor_test::TestTransformComponentHandle::ID());

            const component_editor_test::TestTransformComponent::SetupParam* transform_setup_param = 
                dynamic_cast<const component_editor_test::TestTransformComponent::SetupParam*>(&setup_param());
            ASSERT_NE(transform_setup_param, nullptr);
            EXPECT_FLOAT_EQ(transform_setup_param->position.x, 0.0f);
            EXPECT_FLOAT_EQ(transform_setup_param->position.y, 0.0f);
            EXPECT_FLOAT_EQ(transform_setup_param->position.z, 0.0f);
            EXPECT_FLOAT_EQ(transform_setup_param->rotation.x, 0.0f);
            EXPECT_FLOAT_EQ(transform_setup_param->rotation.y, 0.0f);
            EXPECT_FLOAT_EQ(transform_setup_param->rotation.z, 0.0f);
            EXPECT_FLOAT_EQ(transform_setup_param->scale.x, 1.0f);
            EXPECT_FLOAT_EQ(transform_setup_param->scale.y, 1.0f);
            EXPECT_FLOAT_EQ(transform_setup_param->scale.z, 1.0f);
        }
    }

    /*******************************************************************************************************************
     * Edit setup params
    /******************************************************************************************************************/

    std::string EDITED_NAME = "EditedName";
    bool EDITED_ACTIVE_SELF = false;
    uint64_t EDITED_TAG = 42;
    uint64_t EDITED_LAYER = 3;
    DirectX::XMFLOAT3 EDITED_POSITION = DirectX::XMFLOAT3(1.0f, 2.0f, 3.0f);
    DirectX::XMFLOAT3 EDITED_ROTATION = DirectX::XMFLOAT3(10.0f, 20.0f, 30.0f);
    DirectX::XMFLOAT3 EDITED_SCALE = DirectX::XMFLOAT3(2.0f, 2.0f, 2.0f);
    {
        // Create entity archive service command list
        std::unique_ptr<mono_service::ServiceCommandList> command_list 
            = entity_archive_service_proxy->CreateCommandList();
        mono_entity_archive_service::EntityArchiveServiceCommandList* entity_archive_command_list 
            = dynamic_cast<mono_entity_archive_service::EntityArchiveServiceCommandList*>(command_list.get());
        ASSERT_NE(entity_archive_command_list, nullptr);

        // Create entity archive service view
        std::unique_ptr<mono_service::ServiceView> service_view 
            = entity_archive_service_proxy->CreateView();
        mono_entity_archive_service::EntityArchiveServiceView* entity_archive_service_view 
            = dynamic_cast<mono_entity_archive_service::EntityArchiveServiceView*>(service_view.get());
        ASSERT_NE(entity_archive_service_view, nullptr);

        {

            // Get meta component field map
            utility_header::ConstSharedLockedValue<component_editor::FieldMap> meta_field_map = 
                entity_archive_service_view->GetComponentFieldMap(
                    component_editor_test::TestMetaComponentHandle::ID());

            // Iterate through fields and edit values
            for (const auto& [field_name, field_info] : meta_field_map())
            {
                // Edit field based on its type and name
                if (field_info.type_name == "std::string" && field_name == "name")
                {
                    entity_archive_command_list->EditSetupParamField(
                        test_entity, component_editor_test::TestMetaComponentHandle::ID(), field_name, EDITED_NAME);
                }
                else if (field_info.type_name == "bool" && field_name == "active_self")
                {
                    entity_archive_command_list->EditSetupParamField(
                        test_entity, component_editor_test::TestMetaComponentHandle::ID(), field_name, EDITED_ACTIVE_SELF);
                }
                else if (field_info.type_name == "uint64_t" && field_name == "tag")
                {
                    entity_archive_command_list->EditSetupParamField(
                        test_entity, component_editor_test::TestMetaComponentHandle::ID(), field_name, EDITED_TAG);
                }
                else if (field_info.type_name == "uint64_t" && field_name == "layer")
                {
                    entity_archive_command_list->EditSetupParamField(
                        test_entity, component_editor_test::TestMetaComponentHandle::ID(), field_name, EDITED_LAYER);
                }
            }

        }

        {
            // Get transform component field map
            utility_header::ConstSharedLockedValue<component_editor::FieldMap> transform_field_map = 
                entity_archive_service_view->GetComponentFieldMap(
                    component_editor_test::TestTransformComponentHandle::ID());

            // Iterate through fields and edit values
            for (const auto& [field_name, field_info] : transform_field_map())
            {
                // Edit field based on its type and name
                if (field_info.type_name == "DirectX::XMFLOAT3" && field_name == "position")
                {
                    entity_archive_command_list->EditSetupParamField(
                        test_entity, component_editor_test::TestTransformComponentHandle::ID(), field_name, EDITED_POSITION);
                }
                else if (field_info.type_name == "DirectX::XMFLOAT3" && field_name == "rotation")
                {
                    entity_archive_command_list->EditSetupParamField(
                        test_entity, component_editor_test::TestTransformComponentHandle::ID(), field_name, EDITED_ROTATION);
                }
                else if (field_info.type_name == "DirectX::XMFLOAT3" && field_name == "scale")
                {
                    entity_archive_command_list->EditSetupParamField(
                        test_entity, component_editor_test::TestTransformComponentHandle::ID(), field_name, EDITED_SCALE);
                }
            }
            
        }
        
        // Submit command list
        entity_archive_service_proxy->SubmitCommandList(std::move(command_list));
    }

    /*******************************************************************************************************************
     * Update service
    /******************************************************************************************************************/

    service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        // Get the service
        mono_service::Service& service 
            = registry.Get(mono_entity_archive_service::EntityArchiveServiceHandle::ID());

        // Pre-update
        result = service.PreUpdate();
        ASSERT_TRUE(result);

        // Update
        result = service.Update();
        ASSERT_TRUE(result);

        // Post-update
        result = service.PostUpdate();
        ASSERT_TRUE(result);
    });

    /*******************************************************************************************************************
     * Verify edited setup params
    /******************************************************************************************************************/

    {
        // Create entity archive service view
        std::unique_ptr<mono_service::ServiceView> service_view 
            = entity_archive_service_proxy->CreateView();
        mono_entity_archive_service::EntityArchiveServiceView* entity_archive_service_view 
            = dynamic_cast<mono_entity_archive_service::EntityArchiveServiceView*>(service_view.get());
        ASSERT_NE(entity_archive_service_view, nullptr);

        // Get and verify edited test meta component setup param
        {
            utility_header::ConstSharedLockedValue<ecs::Component::SetupParam> setup_param = 
                entity_archive_service_view->GetSetupParam(
                    test_entity, component_editor_test::TestMetaComponentHandle::ID());

            const component_editor_test::TestMetaComponent::SetupParam* meta_setup_param = 
                dynamic_cast<const component_editor_test::TestMetaComponent::SetupParam*>(&setup_param());
            ASSERT_NE(meta_setup_param, nullptr);
            EXPECT_EQ(meta_setup_param->name, EDITED_NAME);
            EXPECT_EQ(meta_setup_param->active_self, EDITED_ACTIVE_SELF);
            EXPECT_EQ(meta_setup_param->tag, EDITED_TAG);
            EXPECT_EQ(meta_setup_param->layer, EDITED_LAYER);
        }

        // Get and verify edited test transform component setup param
        {
            utility_header::ConstSharedLockedValue<ecs::Component::SetupParam> setup_param = 
                entity_archive_service_view->GetSetupParam(
                    test_entity, component_editor_test::TestTransformComponentHandle::ID());

            const component_editor_test::TestTransformComponent::SetupParam* transform_setup_param = 
                dynamic_cast<const component_editor_test::TestTransformComponent::SetupParam*>(&setup_param());
            ASSERT_NE(transform_setup_param, nullptr);
            EXPECT_FLOAT_EQ(transform_setup_param->position.x, EDITED_POSITION.x);
            EXPECT_FLOAT_EQ(transform_setup_param->position.y, EDITED_POSITION.y);
            EXPECT_FLOAT_EQ(transform_setup_param->position.z, EDITED_POSITION.z);
            EXPECT_FLOAT_EQ(transform_setup_param->rotation.x, EDITED_ROTATION.x);
            EXPECT_FLOAT_EQ(transform_setup_param->rotation.y, EDITED_ROTATION.y);
            EXPECT_FLOAT_EQ(transform_setup_param->rotation.z, EDITED_ROTATION.z);
            EXPECT_FLOAT_EQ(transform_setup_param->scale.x, EDITED_SCALE.x);
            EXPECT_FLOAT_EQ(transform_setup_param->scale.y, EDITED_SCALE.y);
            EXPECT_FLOAT_EQ(transform_setup_param->scale.z, EDITED_SCALE.z);
        }
    }

    /*******************************************************************************************************************
     * Cleanup
    /******************************************************************************************************************/

    service_registry.reset();
}