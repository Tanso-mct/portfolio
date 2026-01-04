#pragma once

#include <DirectXMath.h>

#include "ecs/include/component.h"
#include "mono_service/include/service.h"
#include "transform_evaluator/include/transform_handle.h"
#include "mono_transform_extension/include/dll_config.h"

namespace mono_transform_extension
{

// The handle class for the component
class MONO_TRANSFORM_EXT_DLL TransformComponentHandle : public ecs::ComponentHandle<TransformComponentHandle> {};

// The component class
class MONO_TRANSFORM_EXT_DLL TransformComponent :
    public ecs::Component
{
public:
    TransformComponent(std::unique_ptr<mono_service::ServiceProxy> transform_service_proxy);
    virtual ~TransformComponent() override;

    class SetupParam : //REFLECTABLE_CLASS_BEGIN// 
        public ecs::Component::SetupParam
    {
    public:
        SetupParam() :
            position(0.0f, 0.0f, 0.0f),
            rotation(0.0f, 0.0f, 0.0f),
            scale(1.0f, 1.0f, 1.0f)
        {
        }

        virtual ~SetupParam() override = default;

        // Inital position
        DirectX::XMFLOAT3 position; //REFLECTED//

        // Inital rotation (degrees)
        DirectX::XMFLOAT3 rotation; //REFLECTED//

        // Inital scale
        DirectX::XMFLOAT3 scale; //REFLECTED//

    }; //REFLECTABLE_CLASS_END//

    virtual bool Setup(ecs::Component::SetupParam& param) override;
    virtual ecs::ComponentID GetID() const override;
    virtual bool Apply(const ecs::Component::SetupParam& param) override;

    // Get the world position of the Transform
    DirectX::XMFLOAT3 GetWorldPosition() const;

    // Set the world position of the Transform
    void SetWorldPosition(const DirectX::XMFLOAT3& position);

    // Get the world rotation of the Transform
    DirectX::XMFLOAT4 GetWorldRotation() const;

    // Get the world rotation of the Transform in Euler angles (pitch, yaw, roll in degrees)
    DirectX::XMFLOAT3 GetWorldEulerAngles() const;

    // Set the world rotation of the Transform in quaternion
    void SetWorldRotation(const DirectX::XMFLOAT4& rotation);

    // Set the world rotation of the Transform in Euler angles (pitch, yaw, roll in degrees)
    void SetWorldRotation(const DirectX::XMFLOAT3& euler_angles);

    // Get the world scale of the Transform
    DirectX::XMFLOAT3 GetWorldScale() const;

    // Set the world scale of the Transform
    void SetWorldScale(const DirectX::XMFLOAT3& scale);

    // Get the world TRS matrix of the Transform
    DirectX::XMMATRIX GetWorldMatrix() const;

private:
    // The service proxy for the Transform service
    std::unique_ptr<mono_service::ServiceProxy> transform_service_proxy_ = nullptr;

    // The handle of the Transform in the Transform service
    transform_evaluator::TransformHandle transform_handle_ = transform_evaluator::TransformHandle();
};

} // namespace mono_transform_extension