#pragma once

#include <string>
#include <string_view>

#include "ecs/include/component.h"
#include "mono_meta_extension/include/dll_config.h"

namespace mono_meta_extension
{

// The handle class for the component
class MONO_META_EXT_DLL MetaComponentHandle : public ecs::ComponentHandle<MetaComponentHandle> {};

// Default values
constexpr const char* DEFAULT_NAME = "Unnamed";
constexpr bool DEFAULT_ACTIVE_SELF = true;
constexpr uint64_t DEFAULT_TAG = 0;
constexpr uint64_t DEFAULT_LAYER = 0;

// The component class
class MONO_META_EXT_DLL MetaComponent : //REFLECTABLE_COMMENT_BEGIN//
    public ecs::Component
{
public:
    MetaComponent();
    virtual ~MetaComponent() override;

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
    
    // Get the name of the entity
    std::string_view GetName() const { return name_; }

    // Set the name of the entity
    void SetName(std::string_view name) { name_ = name; }

    // Get whether the entity is active
    bool IsActiveSelf() const { return active_self_; }

    // Set whether the entity is active
    void SetActiveSelf(bool active) { active_self_ = active; }

    // Get the tag of the entity
    uint64_t GetTag() const { return tag_; }

    // Add a tag to the entity
    void AddTag(uint64_t tag) { tag_ |= (uint64_t(1) << tag); }

    // Remove a tag from the entity
    void RemoveTag(uint64_t tag) { tag_ &= ~(uint64_t(1) << tag); }

    // Check if the entity has the specified tag
    bool HasTag(uint64_t tag) const { return (tag_ & (uint64_t(1) << tag)) != 0; }

    // Get the layer of the entity
    uint64_t GetLayer() const { return layer_; }

    // Add the entity to a layer
    void AddToLayer(uint64_t layer) { layer_ |= (uint64_t(1) << layer); }

    // Remove the entity from a layer
    void RemoveFromLayer(uint64_t layer) { layer_ &= ~(uint64_t(1) << layer); }

    // Check if the entity is in the specified layer
    bool IsInLayer(uint64_t layer) const { return (layer_ & (uint64_t(1) << layer)) != 0; }

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

} // namespace mono_meta_extension