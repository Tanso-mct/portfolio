#pragma once

#include "ecs/include/component.h"
#include "mono_entity_archive_extension/include/dll_config.h"

namespace mono_entity_archive_extension
{

// The handle class for the component
class MONO_ENTITY_ARCHIVE_EXT_DLL EditorEnabledComponentHandle : public ecs::ComponentHandle<EditorEnabledComponentHandle> {};

// The component class
// This component marks an entity as editor-enabled
class MONO_ENTITY_ARCHIVE_EXT_DLL EditorEnabledComponent : //REFLECTABLE_COMMENT_BEGIN//
    public ecs::Component
{
public:
    EditorEnabledComponent();
    virtual ~EditorEnabledComponent() override;

    class SetupParam : //REFLECTABLE_SETUP_PARAM_BEGIN//
        public ecs::Component::SetupParam
    {
    public:
        SetupParam() = default;
        virtual ~SetupParam() override = default;

    }; //REFLECTABLE_SETUP_PARAM_END//

    virtual bool Setup(ecs::Component::SetupParam& param) override;
    virtual ecs::ComponentID GetID() const override;

private:

}; //REFLECTABLE_COMMENT_END//

} // namespace mono_entity_archive_extension