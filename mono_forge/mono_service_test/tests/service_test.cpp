#include "mono_service_test/pch.h"

#include "mono_service/include/service.h"
#include "mono_service/include/thread_affinity.h"
#include "mono_service/include/service_registry.h"
#include "mono_service/include/service_importer.h"
#include "mono_service/include/service_command.h"
#include "mono_service/include/service_view.h"

namespace mono_service_test
{

constexpr size_t TEST_SERVICE_COMMAND_QUEUE_BUFFER_COUNT = 2;

class TestServiceCommandList : 
    public mono_service::ServiceCommandList
{
public:
    TestServiceCommandList() = default;
    virtual ~TestServiceCommandList() override = default;
};

class TestServiceView : 
    public mono_service::ServiceView
{
public:
    TestServiceView(const mono_service::ServiceAPI& service_api) :
        mono_service::ServiceView(service_api)
    {
    }

    virtual ~TestServiceView() override = default;
};

class TestServiceAPI : 
    public mono_service::ServiceAPI
{
public:
    TestServiceAPI() = default;
    virtual ~TestServiceAPI() override = default;
};

class TestService : 
    public mono_service::Service,
    private TestServiceAPI
{
public:
    TestService(mono_service::ServiceThreadAffinityID thread_affinity_id)
        : Service(thread_affinity_id)
    {
    }

    virtual ~TestService() override = default;

    class SetupParam : 
        public Service::SetupParam
    {
    public:
        SetupParam() : 
            Service::SetupParam(TEST_SERVICE_COMMAND_QUEUE_BUFFER_COUNT)
        {
        }

        virtual ~SetupParam() override = default;

        bool setup_flag = false;
    };

    virtual bool Setup(Service::SetupParam& param) override
    {
        Service::Setup(param); // Call base class Setup

        SetupParam* test_param = dynamic_cast<SetupParam*>(&param);
        if (test_param == nullptr)
            return false;

        std::cout << "Setting up with flag: " << test_param->setup_flag << std::endl;
        flag_ = test_param->setup_flag;

        return true; // Successfully set up
    }

    virtual bool PreUpdate() override
    {
        BeginFrame();

        Service::PreUpdate(); // Call base class PreUpdate

        std::cout << "PreUpdate called. Value: " << value_ << ", Flag: " << flag_ << std::endl;
        return true; // Successfully pre-updated
    }

    virtual bool Update() override
    {
        Service::Update(); // Call base class Update

        std::cout << "Update called. Value: " << value_ << ", Flag: " << flag_ << std::endl;
        return true; // Successfully updated
    }

    virtual bool PostUpdate() override
    {
        Service::PostUpdate(); // Call base class PostUpdate

        std::cout << "PostUpdate called. Value: " << value_ << ", Flag: " << flag_ << std::endl;

        EndFrame();

        return true; // Successfully post-updated
    }

    virtual std::unique_ptr<mono_service::ServiceCommandList> CreateCommandList() override
    {
        return std::make_unique<TestServiceCommandList>();
    }

    virtual std::unique_ptr<mono_service::ServiceView> CreateView() override
    {
        return std::make_unique<TestServiceView>(GetAPI());
    }

    int GetValue() const
    {
        return value_;
    }

    bool GetFlag() const
    {
        return flag_;
    }

protected:
    const mono_service::ServiceAPI& GetAPI() const
    {
        return static_cast<const mono_service::ServiceAPI&>(*this);
    }

private:
    int value_ = 0;
    bool flag_ = false;
};

struct MainThreadTag {};

} // namespace mono_service_test

TEST(Service, Basic)
{
    // Create singleton service id generator
    std::unique_ptr<mono_service::ServiceIDGenerator> service_id_generator 
        = std::make_unique<mono_service::ServiceIDGenerator>();

    // Create singleton service thread affinity id generator
    std::unique_ptr<mono_service::ServiceThreadAffinityIDGenerator> thread_affinity_id_generator 
        = std::make_unique<mono_service::ServiceThreadAffinityIDGenerator>();

    // Create service registry
    std::unique_ptr<mono_service::ServiceRegistry> service_registry 
        = std::make_unique<mono_service::ServiceRegistry>();

    // Create service importer
    std::unique_ptr<mono_service::ServiceImporter> service_importer 
        = std::make_unique<mono_service::ServiceImporter>(*service_registry);

    // Import the service into the registry
    constexpr bool SETUP_FLAG = true;
    {
        // Instantiate a service with main thread affinity
        std::unique_ptr<mono_service_test::TestService> test_service 
            = std::make_unique<mono_service_test::TestService>(
                mono_service::ServiceThreadAffinityHandle<mono_service_test::MainThreadTag>::ID());

        // Create setup parameters
        mono_service_test::TestService::SetupParam setup_param;
        setup_param.setup_flag = SETUP_FLAG;

        // Import the service
        bool import_result = service_importer->Import(
            std::move(test_service), mono_service::ServiceHandle<mono_service_test::TestService>::ID(), setup_param);
        ASSERT_TRUE(import_result);
    }

    // Retrieve the service from the registry
    service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        // Check if the service is registered
        bool contains 
            = registry.Contains(mono_service::ServiceHandle<mono_service_test::TestService>::ID());
        ASSERT_TRUE(contains);

        // Get the service
        mono_service::Service& service 
            = registry.Get(mono_service::ServiceHandle<mono_service_test::TestService>::ID());

        // Create service proxy
        std::unique_ptr<mono_service::ServiceProxy> service_proxy = service.CreateServiceProxy();

        // Create command list
        std::unique_ptr<mono_service::ServiceCommandList> service_command_list 
            = service_proxy->CreateCommandList();

        // Enqueue command list
        service_proxy->SubmitCommandList(std::move(service_command_list));

        // Call update functions
        bool pre_update_result = service.PreUpdate();
        ASSERT_TRUE(pre_update_result);

        bool update_result = service.Update();
        ASSERT_TRUE(update_result);

        bool post_update_result = service.PostUpdate();
        ASSERT_TRUE(post_update_result);
    });
}