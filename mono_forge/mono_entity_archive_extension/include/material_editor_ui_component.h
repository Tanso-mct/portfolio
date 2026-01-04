#pragma once

#include "ecs/include/component.h"
#include "mono_entity_archive_extension/include/dll_config.h"

namespace mono_entity_archive_extension
{

// The handle class for the component
class MONO_ENTITY_ARCHIVE_EXT_DLL MaterialEditorUIComponentHandle : public ecs::ComponentHandle<MaterialEditorUIComponentHandle> {};

// The component class
class MONO_ENTITY_ARCHIVE_EXT_DLL MaterialEditorUIComponent : //REFLECTABLE_COMMENT_BEGIN//
    public ecs::Component
{
public:
    MaterialEditorUIComponent();
    virtual ~MaterialEditorUIComponent() override;

    class SetupParam : //REFLECTABLE_SETUP_PARAM_BEGIN//
        public ecs::Component::SetupParam
    {
    public:
        SetupParam() = default;
        virtual ~SetupParam() override = default;

        int example_field; //REFLECTABLE_FIELD//

    }; //REFLECTABLE_SETUP_PARAM_END//

    virtual bool Setup(ecs::Component::SetupParam& param) override;
    virtual bool Apply(const ecs::Component::SetupParam& param) override;
    virtual ecs::ComponentID GetID() const override;

private:

}; //REFLECTABLE_COMMENT_END//

} // namespace mono_entity_archive_extension