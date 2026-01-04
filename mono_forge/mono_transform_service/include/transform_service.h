#pragma once

#include <DirectXMath.h>

#include "transform_evaluator/include/transform_manager.h"
#include "mono_service/include/service.h"
#include "mono_service/include/service_registry.h"

#include "mono_transform_service/include/dll_config.h"

namespace mono_transform_service
{

// The transform_service API
// It provides access to transform_service internals for commands
class MONO_TRANSFORM_SERVICE_DLL TransformServiceAPI :
    public mono_service::ServiceAPI
{
public:
    TransformServiceAPI() = default;
    virtual ~TransformServiceAPI() = default;

    // Get the transform adder
    virtual transform_evaluator::TransformAdder& GetTransformAdder() = 0;

    // Get the transform eraser
    virtual transform_evaluator::TransformEraser& GetTransformEraser() = 0;

    // Get the world matrix of a Transform using its Handle
    virtual DirectX::XMMATRIX GetWorldMatrix(const transform_evaluator::TransformHandle& handle) const = 0;

    // Get the translation of a Transform using its Handle
    virtual DirectX::XMFLOAT3 GetTranslation(const transform_evaluator::TransformHandle& handle) const = 0;

    // Get the rotation of a Transform using its Handle
    virtual DirectX::XMFLOAT4 GetRotation(const transform_evaluator::TransformHandle& handle) const = 0;

    // Get the scale of a Transform using its Handle
    virtual DirectX::XMFLOAT3 GetScale(const transform_evaluator::TransformHandle& handle) const = 0;
};

// The number of command queue buffers for transform_service
constexpr size_t SERVICE_COMMAND_QUEUE_BUFFER_COUNT = 2;

// The transform service handle type
class MONO_TRANSFORM_SERVICE_DLL TransformServiceHandle : public mono_service::ServiceHandle<TransformServiceHandle> {};

// The transform_service class
class MONO_TRANSFORM_SERVICE_DLL TransformService :
    public mono_service::Service,
    private TransformServiceAPI
{
public:
    TransformService(mono_service::ServiceThreadAffinityID thread_affinity_id);
    virtual ~TransformService();

    /*******************************************************************************************************************
     * Service
    /******************************************************************************************************************/

    class SetupParam :
        public mono_service::Service::SetupParam
    {
    public:
        SetupParam() :
            mono_service::Service::SetupParam(SERVICE_COMMAND_QUEUE_BUFFER_COUNT)
        {
        }

        virtual ~SetupParam() override = default;
    };
    virtual bool Setup(mono_service::Service::SetupParam& param) override;
    virtual bool PreUpdate() override;
    virtual bool Update() override;
    virtual bool PostUpdate() override;
    virtual std::unique_ptr<mono_service::ServiceCommandList> CreateCommandList() override;
    virtual std::unique_ptr<mono_service::ServiceView> CreateView() override;

protected:
    /*******************************************************************************************************************
     * TransformService API
    /******************************************************************************************************************/

    TransformServiceAPI& GetAPI() { return *this; }
    const TransformServiceAPI& GetAPI() const { return *this; }

    virtual transform_evaluator::TransformAdder& GetTransformAdder() override;
    virtual transform_evaluator::TransformEraser& GetTransformEraser() override;

    virtual DirectX::XMMATRIX GetWorldMatrix(const transform_evaluator::TransformHandle& handle) const override;
    virtual DirectX::XMFLOAT3 GetTranslation(const transform_evaluator::TransformHandle& handle) const override;
    virtual DirectX::XMFLOAT4 GetRotation(const transform_evaluator::TransformHandle& handle) const override;
    virtual DirectX::XMFLOAT3 GetScale(const transform_evaluator::TransformHandle& handle) const override;

private:
    /*******************************************************************************************************************
     * Transform data manipulation
    /******************************************************************************************************************/

    std::unique_ptr<transform_evaluator::TransformContainer> transform_container_ = nullptr;
    std::unique_ptr<transform_evaluator::TransformManager> transform_manager_ = nullptr;
    std::unique_ptr<transform_evaluator::TransformAdder> transform_adder_ = nullptr;
    std::unique_ptr<transform_evaluator::TransformEraser> transform_eraser_ = nullptr;

};

} // namespace mono_transform_service