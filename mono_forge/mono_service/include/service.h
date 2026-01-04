#pragma once

#include <vector>

#include "class_template/non_copy.h"
#include "class_template/thread_safer.h"

#include "mono_service/include/dll_config.h"
#include "mono_service/include/thread_affinity.h"
#include "mono_service/include/service_command.h"
#include "mono_service/include/service_view.h"

namespace mono_service
{

// Forward declaration
class Service;

// Type alias for service fence
using ServiceProgress = uint64_t;

// The proxy class for services
// It provides limited access to the service for external users
class MONO_SERVICE_DLL ServiceProxy
{
public:
    explicit ServiceProxy(Service& service) : service_(service) {}
    ~ServiceProxy() = default;

    // Create a new command list for the associated service
    std::unique_ptr<ServiceCommandList> CreateCommandList();

    // Submit a command list to the associated service
    ServiceProgress SubmitCommandList(std::unique_ptr<ServiceCommandList> command_list);

    // Create a view for the associated service
    std::unique_ptr<ServiceView> CreateView();

    // Get the current service progress from the associated service
    ServiceProgress GetProgress() const;

    // Clone the service proxy
    std::unique_ptr<ServiceProxy> Clone();

protected:
    Service& service_;
};

// Constant for invalid command queue index
constexpr int INVALID_COMMAND_QUEUE_INDEX = -1;

// The base template class for services
// It has a command queue to handle service commands
class MONO_SERVICE_DLL Service :
    public class_template::NonCopyable,
    public class_template::ThreadSafer
{
public:
    Service(ServiceThreadAffinityID thread_affinity_id);
    virtual ~Service();

    // The base setup parameter class for services
    // Specific services can derive their own setup parameter classes from this
    class SetupParam
    {
    public:
        SetupParam(size_t command_queue_buffer_count) :
            command_queue_buffer_count_(command_queue_buffer_count)
        {
        }
        virtual ~SetupParam() = default;

        // The number of command queue buffers
        const size_t command_queue_buffer_count_;
    };

    // Setup the service with the given setup parameters
    // You must explicitly call setup in the base class within setup in the derived class
    virtual bool Setup(SetupParam& param);

    // Pre-update function called before the main update loop
    // You must explicitly call PreUpdate in the base class within PreUpdate in the derived class
    virtual bool PreUpdate();

    // The main update function for the service
    // It will be called every frame in main loop
    // You must explicitly call Update in the base class within Update in the derived class
    virtual bool Update();

    // Post-update function called after the main update loop
    // You must explicitly call PostUpdate in the base class within PostUpdate in the derived class
    virtual bool PostUpdate();

    // Get the thread affinity ID for the service
    ServiceThreadAffinityID GetThreadAffinityID() const;

    // Get a proxy for the service
    std::unique_ptr<ServiceProxy> CreateServiceProxy();

    // Check if the service has been set up
    bool IsSetup() const;

    // Create a new command list for the service
    // It will be call by ServiceProxy, so this function required thread-safe
    // You can use lock logic in ThreadSafer mix-in class
    virtual std::unique_ptr<ServiceCommandList> CreateCommandList() = 0;

    // Submit a command list to the service's command queue
    // Submitted command lists will be executed in the Update function
    // It will be call by ServiceProxy, so this function required thread-safe
    // You can use lock logic in ThreadSafer mix-in class
    ServiceProgress SubmitCommandList(std::unique_ptr<ServiceCommandList> command_list);

    // Get the current service progress
    // It will be call by ServiceProxy, so this function required thread-safe
    // You can use lock logic in ThreadSafer mix-in class
    ServiceProgress GetProgress() const;

    // Create a view for the service
    // The view provides a way to interact with the service's data and functionality
    // You can create derived classes to implement specific views for different services
    // You can use lock logic in ThreadSafer mix-in class
    virtual std::unique_ptr<ServiceView> CreateView() = 0;

protected:
    // Get the executable command queue
    ServiceCommandQueue& GetExecutableCommandQueue();

    // Begin frame processing
    // You must call this at the beginning of each frame
    void BeginFrame();

    // End frame processing
    // You must call this at the end of each frame
    void EndFrame();
    
private:
    // Whether the service has been set up
    bool setup_ = false;

    // The thread affinity ID for the service
    const ServiceThreadAffinityID thread_affinity_id_;

    // Queue of service commands
    std::vector<std::unique_ptr<ServiceCommandQueue>> command_queues_;

    // Current command queue index for round-robin submission
    int current_command_queue_index_ = 0;

    // Current executable command queue index
    int executable_command_queue_index_ = INVALID_COMMAND_QUEUE_INDEX;

    // The progress value of the service
    ServiceProgress progress_ = 0;

    // Lock held during frame processing
    std::unique_lock<std::shared_mutex> holding_lock_;

    // Mutex for queue operations
    mutable std::shared_mutex queue_mutex_;
};

} // namespace mono_service