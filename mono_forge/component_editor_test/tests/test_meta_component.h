#pragma once

#include "ecs/include/component.h"
#include "component_editor/include/component_adder.h"

namespace component_editor_test
{

// Default values
constexpr const char* DEFAULT_NAME = "Unnamed";
constexpr bool DEFAULT_ACTIVE_SELF = true;
constexpr uint64_t DEFAULT_TAG = 0;
constexpr uint64_t DEFAULT_LAYER = 0;

// The handle class for the component
class TestMetaComponentHandle : public ecs::ComponentHandle<TestMetaComponentHandle> {};

// The component class
class TestMetaComponent : //REFLECTABLE_COMMENT_BEGIN//
    public ecs::Component
{
public:
    TestMetaComponent();
    virtual ~TestMetaComponent() override;

    class SetupParam : //REFLECTABLE_SETUP_PARAM_BEGIN//
        public ecs::Component::SetupParam
    {
    public:
        SetupParam() :
            name(DEFAULT_NAME),
            active_self(DEFAULT_ACTIVE_SELF),
            tag(DEFAULT_TAG),
            layer(DEFAULT_LAYER)
        {
        }

        virtual ~SetupParam() override = default;

        // The name of the entity
        std::string name; //REFLECTABLE_FIELD//

        // Whether the entity is active
        bool active_self; //REFLECTABLE_FIELD//

        // The tag of the entity
        uint64_t tag; //REFLECTABLE_FIELD//

        // The layer of the entity
        uint64_t layer; //REFLECTABLE_FIELD//

    }; //REFLECTABLE_SETUP_PARAM_END//

    virtual bool Setup(ecs::Component::SetupParam& param) override;
    virtual ecs::ComponentID GetID() const override;
    virtual bool Apply(const ecs::Component::SetupParam& param) override;

private:
    // The name of the entity
    std::string name_ = DEFAULT_NAME;

    // Whether the entity is active
    bool active_self_ = DEFAULT_ACTIVE_SELF;

    // The tag of the entity
    uint64_t tag_ = DEFAULT_TAG;

    // The layer of the entity
    uint64_t layer_ = DEFAULT_LAYER;

}; //REFLECTABLE_COMMENT_END//

class TestMetaComponentAdder :
    public component_editor::ComponentAdder
{
public:
    TestMetaComponentAdder() = default;
    virtual ~TestMetaComponentAdder() override = default;
    virtual bool Add(
        ecs::World& world, const ecs::Entity& entity, 
        mono_service::ServiceProxyManager& service_proxy_manager) const override;
};


} // namespace component_editor_test