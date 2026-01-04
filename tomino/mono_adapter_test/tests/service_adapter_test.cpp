#include "mono_adapter_test/pch.h"

#include "mono_graphics_service/include/graphics_service.h"
#include "mono_adapter/include/service_adapter.h"

namespace mono_adapter_test
{

class MockService : public mono_service::Service
{
public:
    MockService(mono_service::ServiceThreadAffinityID thread_affinity_id) :
        mono_service::Service(thread_affinity_id)
    {
    }

    ~MockService() override = default;

    virtual bool Setup(mono_service::Service::SetupParam& param) override { return true; }
    virtual bool PreUpdate() override { return true; }
    virtual bool Update() override { return true; }
    virtual bool PostUpdate() override { return true; }
    virtual std::unique_ptr<mono_service::ServiceCommandList> CreateCommandList() override { return nullptr; }
    virtual std::unique_ptr<mono_service::ServiceView> CreateView() override { return nullptr; }

};

} // namespace mono_adapter_test

TEST(ServiceAdapter, GraphicsServiceAdapter)
{
    // Create a mock graphics service
    mono_adapter_test::MockService graphics_service(0);

    // Create the graphics service adapter
    mono_adapter::GraphicsServiceAdapter adapter(graphics_service);

    // Verify that the adapter returns the correct underlying service
    EXPECT_EQ(&adapter.GetGraphicsService(), &graphics_service);
}