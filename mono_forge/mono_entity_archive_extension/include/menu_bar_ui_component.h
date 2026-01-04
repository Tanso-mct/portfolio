#pragma once

#include "ecs/include/entity.h"
#include "ecs/include/component.h"
#include "mono_entity_archive_extension/include/dll_config.h"

namespace mono_entity_archive_extension
{

// The handle class for the component
class MONO_ENTITY_ARCHIVE_EXT_DLL MenuBarUIComponentHandle : public ecs::ComponentHandle<MenuBarUIComponentHandle> {};

// The component class
class MONO_ENTITY_ARCHIVE_EXT_DLL MenuBarUIComponent : //REFLECTABLE_COMMENT_BEGIN//
    public ecs::Component
{
public:
    MenuBarUIComponent();
    virtual ~MenuBarUIComponent() override;

    class SetupParam : //REFLECTABLE_SETUP_PARAM_BEGIN//
        public ecs::Component::SetupParam
    {
    public:
        SetupParam() = default;
        virtual ~SetupParam() override = default;

        // The entity which holds the hierarchy UI
        ecs::Entity hierarchy_entity_ = ecs::Entity();

        // The entity which holds the inspector UI
        ecs::Entity inspector_entity_ = ecs::Entity();

        // The entity which holds the asset browser UI
        ecs::Entity asset_browser_entity_ = ecs::Entity();

        // The entity which holds the material editor UI
        ecs::Entity material_editor_entity_ = ecs::Entity();

    }; //REFLECTABLE_SETUP_PARAM_END//

    virtual bool Setup(ecs::Component::SetupParam& param) override;
    virtual bool Apply(const ecs::Component::SetupParam& param) override;
    virtual ecs::ComponentID GetID() const override;

    // Get the entity which holds the hierarchy UI
    ecs::Entity GetHierarchyEntity() const { return hierarchy_entity_; }

    // Get the entity which holds the inspector UI
    ecs::Entity GetInspectorEntity() const { return inspector_entity_; }

    // Get the entity which holds the asset browser UI
    ecs::Entity GetAssetBrowserEntity() const { return asset_browser_entity_; }

    // Get the entity which holds the material editor UI
    ecs::Entity GetMaterialEditorEntity() const { return material_editor_entity_; }

private:
    // The entity which holds the hierarchy UI
    ecs::Entity hierarchy_entity_ = ecs::Entity();

    // The entity which holds the inspector UI
    ecs::Entity inspector_entity_ = ecs::Entity();

    // The entity which holds the asset browser UI
    ecs::Entity asset_browser_entity_ = ecs::Entity();

    // The entity which holds the material editor UI
    ecs::Entity material_editor_entity_ = ecs::Entity();

}; //REFLECTABLE_COMMENT_END//

} // namespace mono_entity_archive_extension