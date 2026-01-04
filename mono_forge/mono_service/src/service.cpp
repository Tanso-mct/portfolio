#include "mono_service/src/pch.h"
#include "mono_service/include/service.h"

#include "utility_header/logger.h"

namespace mono_service
{

std::unique_ptr<ServiceCommandList> ServiceProxy::CreateCommandList()
{
    return service_.CreateCommandList();
}

ServiceProgress ServiceProxy::SubmitCommandList(std::unique_ptr<ServiceCommandList> command_list)
{
    return service_.SubmitCommandList(std::move(command_list));
}

std::unique_ptr<ServiceView> mono_service::ServiceProxy::CreateView()
{
    return service_.CreateView();
}

ServiceProgress ServiceProxy::GetProgress() const
{
    return service_.GetProgress();
}

std::unique_ptr<ServiceProxy> mono_service::ServiceProxy::Clone()
{
    // Create a new ServiceProxy for the same service
    return service_.CreateServiceProxy();
}

Service::Service(ServiceThreadAffinityID thread_affinity_id) :
    thread_affinity_id_(thread_affinity_id)
{
}

Service::~Service()
{
}

bool Service::Setup(SetupParam& param)
{
    assert(!setup_ && "Service is already set up.");

    // Create service command queues
    for (size_t i = 0; i < param.command_queue_buffer_count_; ++i)
        command_queues_.emplace_back(std::make_unique<ServiceCommandQueue>());

    // Mark as set up
    setup_ = true;

    return true; // Setup successful
}

bool Service::PreUpdate()
{
    assert(setup_ && "Service must be set up before PreUpdate.");
    return true; // PreUpdate successful
}

bool Service::Update()
{
    assert(holding_lock_.owns_lock() && "Must hold unique lock during Update.");
    assert(setup_ && "Service must be set up before Update.");
    return true; // Update successful
}

bool Service::PostUpdate()
{
    assert(setup_ && "Service must be set up before PostUpdate.");
    return true; // PostUpdate successful
}

ServiceThreadAffinityID Service::GetThreadAffinityID() const
{
    return thread_affinity_id_;
}

std::unique_ptr<ServiceProxy> Service::CreateServiceProxy()
{
    return std::make_unique<ServiceProxy>(*this);
}

bool Service::IsSetup() const
{
    return setup_;
}

ServiceProgress Service::SubmitCommandList(std::unique_ptr<ServiceCommandList> command_list)
{
    // Lock for unique access in queue operations
    std::unique_lock<std::shared_mutex> queue_lock = std::unique_lock<std::shared_mutex>(queue_mutex_);

    assert(setup_ && "Service must be set up before submitting command lists.");
    assert(command_list != nullptr && "Command list must not be null.");

    // Enqueue the command list to the current command queue
    command_queues_[current_command_queue_index_]->EnqueueCommandList(std::move(command_list));

    // Return the current progress
    return progress_;
}

ServiceProgress Service::GetProgress() const
{
    // Lock for shared access
    std::shared_lock<std::shared_mutex> lock = LockShared();

    assert(setup_ && "Service must be set up before getting progress.");
    return progress_;
}

ServiceCommandQueue& Service::GetExecutableCommandQueue()
{
    assert(holding_lock_.owns_lock() && "Must hold unique lock to get executable command queue.");
    assert(setup_ && "Service must be set up before getting command queue.");
    return *command_queues_[executable_command_queue_index_];
}

void Service::BeginFrame()
{
    // Lock for unique access in queue operations
    std::unique_lock<std::shared_mutex> queue_lock = std::unique_lock<std::shared_mutex>(queue_mutex_);

    // Set unique lock for thread-safe access
    holding_lock_ = LockUnique();

    // Store the current executable command queue index
    executable_command_queue_index_ = current_command_queue_index_;

    // Switch to next command queue
    current_command_queue_index_ = (current_command_queue_index_ + 1) % command_queues_.size();

    // Check the new record queue is empty
    assert(
        command_queues_[current_command_queue_index_]->IsEmpty() && 
        "Record command queue is not empty at the beginning of the frame. Did you forget to execute commands?");
}

void Service::EndFrame()
{
    // Lock for unique access in queue operations
    std::unique_lock<std::shared_mutex> queue_lock = std::unique_lock<std::shared_mutex>(queue_mutex_);

    // Clear the executable command queue index
    executable_command_queue_index_ = INVALID_COMMAND_QUEUE_INDEX;

    // Update the progress value
    ++progress_;

    // Release the unique lock
    holding_lock_.unlock();
}

} // namespace mono_service