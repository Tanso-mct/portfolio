#pragma once

#include <DirectXMath.h>
#include "ecs/include/component.h"
#include "component_editor/include/component_adder.h"

namespace component_editor_test
{

// The handle class for the component
class TestTransformComponentHandle : public ecs::ComponentHandle<TestTransformComponentHandle> {};

// The component class
class TestTransformComponent : //REFLECTABLE_COMMENT_BEGIN//
    public ecs::Component
{
public:
    TestTransformComponent();
    virtual ~TestTransformComponent() override;

    class SetupParam : //REFLECTABLE_SETUP_PARAM_BEGIN//
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

    }; //REFLECTABLE_SETUP_PARAM_END//

    virtual bool Setup(ecs::Component::SetupParam& param) override;
    virtual ecs::ComponentID GetID() const override;
    virtual bool Apply(const ecs::Component::SetupParam& param) override;

private:
    // The position of the Transform
    DirectX::XMFLOAT3 position_ = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

    // The rotation of the Transform (degrees)
    DirectX::XMFLOAT3 rotation_ = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

    // The scale of the Transform
    DirectX::XMFLOAT3 scale_    = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

}; //REFLECTABLE_COMMENT_END//

class TestTransformComponentAdder : 
    public component_editor::ComponentAdder
{
public:
    TestTransformComponentAdder() = default;
    virtual ~TestTransformComponentAdder() override = default;
    virtual bool Add(
        ecs::World& world, const ecs::Entity& entity,
        mono_service::ServiceProxyManager& service_proxy_manager) const override;
};

} // namespace component_editor_test