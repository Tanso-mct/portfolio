#include "mono_transform_service/src/pch.h"
#include "mono_transform_service/include/transform_service.h"

using namespace DirectX;

#include "mono_transform_service/include/transform_service_command_list.h"
#include "mono_transform_service/include/transform_service_view.h"

namespace mono_transform_service
{

TransformService::TransformService(mono_service::ServiceThreadAffinityID thread_affinity_id) :
    mono_service::Service(thread_affinity_id)
{
}

TransformService::~TransformService()
{
    /*******************************************************************************************************************
     * Transform data manipulation Cleanup
    /******************************************************************************************************************/

    transform_eraser_.reset();
    transform_adder_.reset();
    transform_manager_.reset();
    transform_container_.reset();
}

bool TransformService::Setup(mono_service::Service::SetupParam& param)
{
    // Call base class Setup
    if (!mono_service::Service::Setup(param))
        return false;

    /*******************************************************************************************************************
     * Transform data manipulation
    /******************************************************************************************************************/

    // Create TransformContainer
    transform_container_ = std::make_unique<transform_evaluator::TransformContainer>();

    // Create TransformManager
    transform_manager_ = std::make_unique<transform_evaluator::TransformManager>(*transform_container_);

    // Create TransformAdder
    transform_adder_ = std::make_unique<transform_evaluator::TransformAdder>(*transform_container_);

    // Create TransformEraser
    transform_eraser_ = std::make_unique<transform_evaluator::TransformEraser>(*transform_container_);

    return true; // Setup successful
}

bool TransformService::PreUpdate()
{
    // Call base class PreUpdate
    if (!mono_service::Service::PreUpdate())
        return false;

    return true;
}

bool TransformService::Update()
{
    // Begin frame update
    BeginFrame();

    // Call base class Update
    if (!mono_service::Service::Update())
        return false;

    bool result = false;

    // Execute all enqueued command lists
    // TODO: Add logic to determine the value considering the parent-child relationship
    while (!GetExecutableCommandQueue().IsEmpty())
    {
        // Dequeue command list
        std::unique_ptr<mono_service::ServiceCommandList> command_list
            = GetExecutableCommandQueue().DequeueCommandList();

        // Execute all commands in the command list
        for (const auto& command : command_list->GetCommands())
        {
            // Execute command
            result = command->Execute(GetAPI());
            if (!result)
                return false; // Stop update on failure
        }
    }

    // End frame update
    EndFrame();

    return true; // Update successful
}

bool TransformService::PostUpdate()
{
    // Call base class PostUpdate
    if (!mono_service::Service::PostUpdate())
        return false;

    return true;
}

std::unique_ptr<mono_service::ServiceCommandList> TransformService::CreateCommandList()
{
    return std::make_unique<TransformServiceCommandList>();
}

std::unique_ptr<mono_service::ServiceView> TransformService::CreateView()
{
    return std::make_unique<TransformServiceView>(GetAPI());
}

transform_evaluator::TransformAdder& TransformService::GetTransformAdder()
{
    assert(IsSetup() && "TransformService is not set up.");
    return *transform_adder_;
}

transform_evaluator::TransformEraser& TransformService::GetTransformEraser()
{
    assert(IsSetup() && "TransformService is not set up.");
    return *transform_eraser_;
}

XMMATRIX TransformService::GetWorldMatrix(
    const transform_evaluator::TransformHandle &handle) const
{
    assert(IsSetup() && "TransformService is not set up.");

    // Lock for shared access
    std::shared_lock<std::shared_mutex> lock = LockShared();

    // Default world matrix (identity)
    XMMATRIX world_matrix = XMMatrixIdentity();

    transform_evaluator::TransformManager& transform_manager = transform_evaluator::TransformManager::GetInstance();
    transform_manager.WithLock([&](const transform_evaluator::TransformManager& manager)
    {
        // Get the Transform using its Handle
        const transform_evaluator::Transform& transform = manager.GetTransform(handle);
        
        // Get world TRS
        const transform_evaluator::TRS& world_trs = transform.GetWorldTRS();

        // Compute world matrix from TRS
        XMVECTOR translation = XMLoadFloat3(&world_trs.translation);
        XMVECTOR rotation = XMLoadFloat4(&world_trs.rotation);
        XMVECTOR scale = XMLoadFloat3(&world_trs.scale);

        XMMATRIX scale_matrix = XMMatrixScalingFromVector(scale);
        XMMATRIX rotation_matrix = XMMatrixRotationQuaternion(rotation);
        XMMATRIX translation_matrix = XMMatrixTranslationFromVector(translation);

        world_matrix = scale_matrix * rotation_matrix * translation_matrix;
    });

    return world_matrix;
}

DirectX::XMFLOAT3 TransformService::GetTranslation(const transform_evaluator::TransformHandle &handle) const
{
    assert(IsSetup() && "TransformService is not set up.");

    // Lock for shared access
    std::shared_lock<std::shared_mutex> lock = LockShared();

    // Pointer to hold the translation
    DirectX::XMFLOAT3 translation = {};

    transform_evaluator::TransformManager& transform_manager = transform_evaluator::TransformManager::GetInstance();
    transform_manager.WithLock([&](const transform_evaluator::TransformManager& manager)
    {
        // Get the Transform using its Handle
        const transform_evaluator::Transform& transform = manager.GetTransform(handle);
        
        // Get world TRS
        const transform_evaluator::TRS& world_trs = transform.GetWorldTRS();

        translation = world_trs.translation;
    });

    return translation;
}

DirectX::XMFLOAT4 TransformService::GetRotation(const transform_evaluator::TransformHandle &handle) const
{
    assert(IsSetup() && "TransformService is not set up.");

    // Lock for shared access
    std::shared_lock<std::shared_mutex> lock = LockShared();

    // Pointer to hold the rotation
    DirectX::XMFLOAT4 rotation = {};

    transform_evaluator::TransformManager& transform_manager = transform_evaluator::TransformManager::GetInstance();
    transform_manager.WithLock([&](const transform_evaluator::TransformManager& manager)
    {
        // Get the Transform using its Handle
        const transform_evaluator::Transform& transform = manager.GetTransform(handle);
        
        // Get world TRS
        const transform_evaluator::TRS& world_trs = transform.GetWorldTRS();

        rotation = world_trs.rotation;
    });

    return rotation;
}

DirectX::XMFLOAT3 TransformService::GetScale(const transform_evaluator::TransformHandle &handle) const
{
    assert(IsSetup() && "TransformService is not set up.");

    // Lock for shared access
    std::shared_lock<std::shared_mutex> lock = LockShared();

    // Pointer to hold the scale
    DirectX::XMFLOAT3 scale = {};

    transform_evaluator::TransformManager& transform_manager = transform_evaluator::TransformManager::GetInstance();
    transform_manager.WithLock([&](const transform_evaluator::TransformManager& manager)
    {
        // Get the Transform using its Handle
        const transform_evaluator::Transform& transform = manager.GetTransform(handle);
        
        // Get world TRS
        const transform_evaluator::TRS& world_trs = transform.GetWorldTRS();

        scale = world_trs.scale;
    });

    return scale;
}

} // namespace mono_transform_service